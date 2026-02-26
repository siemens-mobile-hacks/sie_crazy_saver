#include <swilib.h>
#include "config.h"

char CFG_PATH[] = "?:\\zbin\\etc\\SieCrazySaver.bcfg";

CONFIG CFG = {
    {CFG_STR_WIN1251, "MP CSM addr", 0, 15},
    #ifdef NEWSGOLD
        #ifdef ELKA
            "A087109C",
        #else
            "A068ED54",
        #endif
    #else
        "A0A29E70",
    #endif
    {CFG_LEVEL, "Illumination", 1, 0},
    {CFG_CHECKBOX, "Enable", 0, 2},
    1,
    {CFG_CHECKBOX, "Override brightness", 0, 2},
    1,
    {CFG_LEVEL, "", 0, 0},
    {CFG_UINT, "Font size clock", 0, 100},
    #ifdef ELKA
        100,
    #else
        52,
    #endif
    {CFG_UINT, "Font size track", 0, 100},
    #ifdef ELKA
        28,
    #else
        18,
    #endif
    {CFG_UINT, "Font size clock 2", 0, 100},
    #ifdef ELKA
        32,
    #else
        22,
    #endif
};

void InitConfig() {
    CFG_PATH[0] = BCFG_GetDefaultDisk();
    if (BCFG_LoadConfig(CFG_PATH, &CFG, sizeof(CONFIG)) == -1) {
        BCFG_SaveConfig(CFG_PATH, &CFG, sizeof(CONFIG));
    }
}
