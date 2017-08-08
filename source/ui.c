#include "ui.h"
#include "unicode.h"

static drawableScreen_t *bottomScreen;

extern backgroundScreen_t *awaitbg;
extern backgroundScreen_t *workingbg;

void quit(void);

void topScene(int stage)
{
    setScreen(GFX_TOP);
    setTextColor(0xFFFFFFFF); // white

    if (stage == 0) {
        char* string =
            "Place source amiibo and press " FONT_A "\n"
            "          Press START to exit";

        renderText(100.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else

    if (stage == 1) {
        char* string =
            "Awaiting source amiibo";

        renderText(135.0f, 118.0f, 0.5f, 0.5, false, string, 0);
    } else

    if (stage == 2) {
        char* string =
            "Place destination amiibo and press " FONT_A "\n"
            "             Press START to exit";

        renderText(90.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else

    if (stage == 3) {
        char* string =
            "Awaiting destination amiibo";

        renderText(130.0f, 118.0f, 0.5f, 0.5, false, string, 0);
    } else

    if (stage == 4) {
        char* string =
            "Success! Press START to exit";
        renderText(115.0f, 118.0f, 0.5f, 0.5, false, string, 0);
    }
    updateScreen();
}

// Function to set bottom screen
void bottomScene(int scene)
{
    setScreen(GFX_BOTTOM);
    bottomScreen = newDrawableScreen(scene == 0 ?  awaitbg : workingbg);
    bottomScreen->draw(bottomScreen);
    updateScreen();
}

void topSceneError(int stage)
{
    svcSleepThread(3000000000);
    setScreen(GFX_TOP);
    setTextColor(0xFFFFFFFF); // white
    renderText(100.0f, 104.0f, 0.5f, 0.5f, false, "ERROR", 0);

    if (stage == 0) {
        char* string =
            "Failed to start NFC";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 1) {
        char* string =
            "Failed to get NFC state";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 2) {
        char* string =
            "Failed to initialize NFC";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 3) {
        char* string =
            "Failed to get NFC info";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 4) {
        char* string =
            "Failed to open NFC data";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 5) {
        char* string =
            "No app data found";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage ==6) {
        char* string =
            "Couldn't find AppID\n"
            "Please message me with\ngame name of amiibo";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 7) {
        char* string =
            "Please set up this amiibo\nwith the Settings app";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 8) {
        char* string =
            "Failed to read amiibo Settings";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 9) {
        char* string =
            "Failed to read Amiibo Config";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 10) {
        char* string =
            "Failed to read amiibo AppData";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 11) {
        char* string =
            "Failed to write amiibo";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    } else
    if (stage == 12) {
        char* string =
            "Failed to update amiibo";
        renderText(110.0f, 118.0f, 0.5f, 0.5f, false, string, 0);
    }

    renderText(110.0f, 118.0f, 0.5f, 0.5f, false, "\n\n\nQuiting app", 0);
    updateScreen();
    quit();
}
