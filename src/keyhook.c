#include <swilib.h>
#include <sie/sie.h>
#include "mp.h"
#include "gui.h"

extern int CFG_ENABLE_ILLUMINATION;
extern SS_GUI SS;

int KeyHook(int submsg, int msg) {
    if (IsGuiOnTop(SS.id)) {
        if (!IsUnlocked()) {
            if (msg == KEY_DOWN) {
                if (!IsMPOn() || !CFG_ENABLE_ILLUMINATION) {
                    TempLightOn(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 0x7FFF);
                }
                if (submsg != '#') {
                    return KEYHOOK_BREAK;
                }
            }
        } else {
            if (msg == KEY_DOWN) {
                GeneralFunc_flag1(SS.id, 1);
                return KEYHOOK_BREAK;
            }
        }
    }
    return KEYHOOK_NEXT;
}

void AddKeyHook() {
    AddKeybMsgHook(KeyHook);
}

void RemoveKeyHook() {
    RemoveKeybMsgHook(KeyHook);
}
