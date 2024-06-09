#include <swilib.h>

void BacklightOn(int level) {
    if (level == -1) {
        if (!SettingsAE_Read(&level, SETTINGS_ID_SETUP, NULL, "DISPLAY_ILLUMINATION")) {
            level = 100;
        }
    }
    SetIllumination(ILLUMINATION_DEV_KEYBOARD, 1, level, 1000);
    SetIllumination(ILLUMINATION_DEV_DISPLAY, 1, level, 1000);
}

void BacklightOff() {
    SetIllumination(ILLUMINATION_DEV_KEYBOARD, 1, 0, 1000);
    SetIllumination(ILLUMINATION_DEV_DISPLAY, 1, 0, 1000);
}
