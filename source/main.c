#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "draw.h"
#include "drawableObject.h"
#include "ui.h"
#include "nfc.h"

// Variables for amiibo loop:
bool write = false;

extern NFC_TagState prevstate, curstate;
extern u8 appdata[0xD8];
extern NFC_TagInfo taginfo;

// Variables for UI
sprite_t *await;
sprite_t *working;

backgroundScreen_t *awaitbg;
backgroundScreen_t *workingbg;

extern int textVtxArrayPos;

// Function to exit app
void quit(void)
{
    svcSleepThread(3000000000);
    romfsExit();
    drawExit();
    exit(0);
}

// Main function
int main(int argc, char **argv)
{
    // Initialize graphics.
    gfxInitDefault();
    drawInit();
    fontEnsureMapped();

    // Initiate ROMFS
    romfsInit();

    // Read graphics from ROMFS and load them into backgrounds
    newSpriteFromPNG(&await, "romfs:/await.png");
    newSpriteFromPNG(&working, "romfs:/working.png");
    awaitbg = newBackgroundObject(await, NULL, NULL);
    workingbg = newBackgroundObject(working, NULL, NULL);

    // Initialize NFC
    checkNFC();

    // Prompt for source amiibo
    topScene(0);
    // Set bottom screen to waiting
    bottomScene(0);
    // Stop NFC & wait for input
    wait_for_amiibo();


    // Show warning not to remove
    topScene(1);
    // Set bottom screen to working
    bottomScene(1);
    // Start NFC
    startScanning();


    // Read Data Loop
    // Check amiibo
    while (!checkAmiibo(1)) {}

    svcSleepThread(3000000000);
    // Prompt for Destination amiibo
    topScene(2);
    // Set bottom screen to waiting
    bottomScene(0);
    // Stop NFC & wait for input
    wait_for_amiibo();


    // Show warning not to remove
    topScene(3);
    // Set bottom screen to working
    bottomScene(1);
    // Start NFC
    startScanning();



    // Write Data Loop
    // Set write flag for AppData Initialization
    write = true;

    // Check amiibo
    while (!checkAmiibo(3)) {}

    // Program done & scanning stopped
    nfcStopScanning();

    svcSleepThread(3000000000);
    // Show finish screen
    topScene(4);

    // Wait for exit key
    while (aptMainLoop())
    {
        hidScanInput(); //Scan all the inputs.
        u32 kUp = hidKeysUp(); //Get released buttons
        if (kUp & KEY_START)
            quit(); // break in order to return to hbmenu
    }
    return 0;
}
