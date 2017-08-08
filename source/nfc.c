#include <stdbool.h>
#include "nfc.h"
#include "draw.h"
#include "drawableObject.h"
#include "ui.h"

static Result ret = 0;

// Variables for amiibo loop:
NFC_TagState curstate;
NFC_AmiiboSettings amiibosettings;
NFC_AmiiboConfig amiiboconfig;
NFC_TagInfo taginfo;
static Handle nfcHandle;

u32 pos;
u8 appdata[0xD8];
u32 appid = 0x00000000;
u32 sourceAppID = 0x00000000;
//Amiibo APPID database from 3dbrew wiki
u32 appid_db[5] =
    {
       0x10110E00,
       0x0014F000,
       0x00152600,
       0x00132600,
       0x1019C800,
    };

char uidstr[16];
extern bool write;

// Declaration for quit() from main.c
void quit(void);

// Function to start the NFC for scanning
void startScanning(void)
{
    ret = nfcStartScanning(NFC_STARTSCAN_DEFAULTINPUT);
    if (R_FAILED(ret))
        topSceneError(0);
}

// Function to check TAG state
void getTag(void)
{
    ret = nfcGetTagState(&curstate);
    if (R_FAILED(ret))
        topSceneError(1);
}

// Function to initialize and check NFC
void checkNFC(void)
{
    ret = nfcInit(NFC_OpType_NFCTag);
    if (R_FAILED(ret))
        topSceneError(2);

    nfcHandle = nfcGetSessionHandle();
    startScanning();
    getTag();
}

// Function to wait for key press and amiibo precence.
void wait_for_amiibo(void)
{
    nfcStopScanning();
    //ButtonCheck Loop
    while (true) {
        hidScanInput(); //Scan all the inputs.
        u32 kUp = hidKeysUp(); //Get released buttons
        if (kUp & KEY_A)
            break; // break in order to continue
        if (kUp & KEY_START)
            quit();
    }
}

// Function to retrieve AppID
Result AmiiboGetAppID(u32 *appid)
{
    Result ret = 0;
    u32* cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = IPC_MakeHeader(0x402, 0, 0);

    if (R_FAILED(ret = svcSendSyncRequest(nfcHandle)))
        return ret;

    ret = cmdbuf[1];
    *appid = cmdbuf[4];

    return ret; // Should be 0x0
}

// Function loop to check for valid amiibo data and config info
bool checkAmiibo(int stage)
{
    getTag();

    // If current state signals Amiibo being in range
    if (curstate == NFC_TagState_InRange) {
        setScreen(GFX_TOP);
        setTextColor(0xFFFFFFFF); // white
        renderText(125.0f, 118.0f, 0.5f, 0.5, false, "DO NOT REMOVE AMIIBO", 0);
        updateScreen();

        //Clear variables:
        memset(&taginfo, 0, sizeof(NFC_TagInfo));

        ret = nfcGetTagInfo(&taginfo); //Try to get info about NFC Tag.
        if (R_FAILED(ret))
            goto error;

        memset(uidstr, 0, sizeof(uidstr));
        for (pos = 0; pos < 7; pos++)
            snprintf(&uidstr[pos * 2], 3, "%02x", taginfo.id[pos]);

        ret = nfcLoadAmiiboData(); //Try to load Amiibo data.
        if (R_FAILED(ret))
            goto error;


        bool foundAppId = false; //Check if we've found the AppID.
        if (AmiiboGetAppID(&appid) == 0x00000000) { //Fetch AppID from an unknown nfc:m command
            ret = nfcOpenAppData(appid); //Try to open Amiibo App Data with fetched AppID.
            if (R_FAILED(ret)) {
                if (ret == NFC_ERR_APPDATA_UNINITIALIZED) {
                    // If reading, show error, if writing jump to initialization
                    if (write) goto write; else topSceneError(5);
                }
            } else {
                // Stores source AppID for later use with initialization
                sourceAppID = appid;
                foundAppId = true;
            }
        } else {
            for (int i = 0; i < 5; i++) { //Loop thru' AppID database and try to find AppID.
                ret = nfcOpenAppData(appid_db[i]); //Try to open Amiibo App Data with current AppID.
                if (R_FAILED(ret)) {
                    if (ret == NFC_ERR_APPDATA_UNINITIALIZED) {
                        // If reading, show error, if writing jump to initialization
                        if (write) goto write; else topSceneError(5);
                    }
                } else {
                    appid = appid_db[i];
                    // Stores source AppID for later use with initialization
                    sourceAppID = appid;
                    foundAppId = true; //Set AppID Search Status to True
                    break;
                }
            }
        }

        // If we haven't found the amiibo's AppID:
        if (!foundAppId)
            topSceneError(6);

        //Clear variables:
        memset(&amiibosettings, 0, sizeof(NFC_AmiiboSettings));
        memset(&amiiboconfig, 0, sizeof(NFC_AmiiboConfig));

        ret = nfcGetAmiiboSettings(&amiibosettings); //Try to get Amiibo Settings.
        if (R_FAILED(ret)) {
            if (ret == NFC_ERR_AMIIBO_NOTSETUP) //If Amiibo was not set up:
                topSceneError(7);
            else
                topSceneError(8);
        }

        ret = nfcGetAmiiboConfig(&amiiboconfig); //Try to get Amiibo Config.
        if (R_FAILED(ret))
            topSceneError(9);

        if (!write) {
            // Clear appdata variable.
            memset(appdata, 0, sizeof(appdata));

            // Read source amiibo AppData.
            ret = nfcReadAppData(appdata, sizeof(appdata));
            if (R_FAILED(ret))
                goto error;
        }

        // Jump here if amiibo is uninitialized
        write:
        if (write) {
            // Try to Initialize destination amiibo
            ret = nfcInitializeWriteAppData(sourceAppID, appdata, sizeof(appdata));
            if (R_FAILED(ret)) {
                // Try to write Amiibo if already initialized
                ret = nfcWriteAppData(appdata, sizeof(appdata), &taginfo);
                if (R_FAILED(ret))
                    goto error;

                // Try to update Amiibo
                ret = nfcUpdateStoredAmiiboData();
                if (R_FAILED(ret))
                    goto error;
            }
        }
        return true;
    }
    return false;

    // Jump here in case of reading/writing error to try again
    error:
    topScene(stage);
    nfcStopScanning();
    startScanning();
    return false;
}
