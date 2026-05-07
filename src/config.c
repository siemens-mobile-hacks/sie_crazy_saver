#include <swilib.h>
#include "config.h"

char CFG_PATH[] = "?:\\zbin\\etc\\SieCrazySaver.bcfg";

CONFIG CFG = {
    // Clock
    {CFG_LEVEL, "Clock", 1, 0},
    {CFG_CBOX, "Type", 0, 2},
    #ifndef ELKA
        CFG_CLOCK_TYPE_BUILT_IN_DIGITAL,
    #else
        CFG_CLOCK_TYPE_PNG_DIGITAL,
    #endif
    {{"Built‑in digital"}, {"PNG digital"}},
    // Clock->Built-in digital
    {CFG_LEVEL, "Built‑in digital", 1, 0},
    {CFG_UINT, "Digit 0 icon", 0, 2000},
    #ifdef NEWSGOLD
        #ifdef ELKA
            616,
        #else
            608,
        #endif
    #endif
    {CFG_LEVEL, "", 0, 0},
    // Clock->Custom PNG digital
    {CFG_LEVEL, "PNG digital", 1, 0},
    {CFG_STR_UTF8, "Digit background path", 3, 127},
    "0:\\zbin\\img\\SieCrazySaver\\digit_bg.png",
    {CFG_STR_UTF8, "Digits image path", 3, 127},
    "0:\\zbin\\img\\SieCrazySaver\\digits.png",
    {CFG_UINT, "Offset X", 0, 200},
    7,
    {CFG_LEVEL, "", 0, 0},
    {CFG_LEVEL, "", 0, 0},
    // Media player
    {CFG_LEVEL, "Media player", 1, 0},
    {CFG_STR_WIN1251, "CSM addr", 0, 15},
    #ifdef NEWSGOLD
        #ifdef ELKA
            "A087109C",
        #else
            "A068ED54",
        #endif
    #else
        "A0A29E70",
    #endif
    #ifdef NEWSGOLD
    #ifndef ELKA
        {CFG_CHECKBOX, "Detect IMS-700", 0, 2},
        1,
    #endif
    #endif
    {CFG_LEVEL, "", 0, 0},
    // Illumination
    {CFG_LEVEL, "Illumination", 1, 0},
    {CFG_CHECKBOX, "Enable", 0, 2},
    1,
    {CFG_CHECKBOX, "Override brightness", 0, 2},
    1,
    {CFG_LEVEL, "", 0, 0},
};

void InitConfig() {
    CFG_PATH[0] = BCFG_GetDefaultDisk();
    if (BCFG_LoadConfig(CFG_PATH, &CFG, sizeof(CONFIG)) == -1) {
        BCFG_SaveConfig(CFG_PATH, &CFG, sizeof(CONFIG));
    }
}
