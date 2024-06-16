#include <swilib.h>
#include "gui.h"
#include "backlight.h"

extern unsigned int GUI_ID;
extern unsigned int CODE_PROTECTION_CSM_ID;

unsigned int CSM_ID;

static const int minus11 = -11;
static unsigned short maincsm_name_body[140];

typedef struct {
    CSM_RAM csm;
    MAIN_GUI *main_gui;
} MAIN_CSM;

int KeyHook(int submess, int msg) {
    if (msg == KEY_UP) { // fix backlight in code protection gui
        if (!IsUnlocked() && !IsGuiOnTop((int)GUI_ID)) {
            BacklightOn(-1);
        }
    }
    return KEYHOOK_NEXT;
}

static void maincsm_oncreate(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    csm->csm.state = 0;
    csm->csm.unk1 = 0;
    csm->main_gui = CreateCrazyGUI(0);
    AddKeybMsgHook(KeyHook);
}

static void maincsm_onclose(CSM_RAM *data) {
    CloseScreensaver();
    RemoveKeybMsgHook(KeyHook);
    CSM_ID = 0;
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if ((msg->msg == MSG_GUI_DESTROYED) && ((int)msg->data0 == GUI_ID)) {
        csm->csm.state = -3;
    } else if (msg->msg == MSG_CSM_DESTROYED) {
        if ((int)msg->data0 == CODE_PROTECTION_CSM_ID && (int)msg->data1 == 3) {
            KbdUnlock();
            csm->csm.state = -3;
        }
    }
    return 1;
}

static const struct {
    CSM_DESC maincsm;
    WSHDR maincsm_name;
} MAINCSM = {
        {
                maincsm_onmessage,
                maincsm_oncreate,
#ifdef NEWSGOLD
                0,
                0,
                0,
                0,
#endif
                maincsm_onclose,
                sizeof(MAIN_CSM),
                1,
                &minus11
        },
        {
                maincsm_name_body,
                NAMECSM_MAGIC1,
                NAMECSM_MAGIC2,
                0x0,
                139,
                0
        }
};

static void UpdateCSMname(void) {
    wsprintf((WSHDR *)&MAINCSM.maincsm_name, "%t", "Screensaver");
}

void CreateCrazyCSM() {
    MAIN_CSM main_csm;
    LockSched();
    UpdateCSMname();
    CSM_ID = CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
    UnlockSched();
}
