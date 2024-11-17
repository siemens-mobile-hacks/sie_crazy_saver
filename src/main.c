#include <swilib.h>
#include <string.h>
#include <nu_swilib.h>
#include <sie/sie.h>
#include "gui.h"
#include "config_loader.h"

#define ELF_NAME "SieCrazySaver"

typedef struct {
    CSM_RAM csm;
} MAIN_CSM;

extern char CFG_PATH[];
extern unsigned int CSM_ID;

SS_GUI SS;
GUI_METHODS *METHODS_OLD;
GUI_METHODS METHODS_NEW;
unsigned int DAEMON_CSM_ID;

const int minus11 =- 11;
unsigned short maincsm_name_body[140];

void HookGUI(const SS_GUI *ss) {
    int i = 0;
    while (!IsGuiOnTop(ss->id)) {
        if (i >= 3) {
            return;
        }
        NU_Sleep(50);
        i++;
    };
    memcpy(&SS, ss, sizeof(SS_GUI));
    InitData();
    LockSched();
    memcpy(&METHODS_NEW, ss->gui->methods, sizeof(GUI_METHODS));
    METHODS_OLD = ss->gui->methods;
    METHODS_NEW.onRedraw = OnRedraw;
    METHODS_NEW.onClose = OnClose;
    METHODS_NEW.onFocus = OnFocus;
    METHODS_NEW.onUnfocus = OnUnFocus;
    METHODS_NEW.onKey = OnKey;
    METHODS_NEW.onDestroy = OnDestroy;
    ss->gui->methods = &METHODS_NEW;
    UnlockSched();
    DirectRedrawGUI_ID(ss->id);
}

static int CSM_OnMessage(CSM_RAM *data, GBS_MSG *msg) {
    if (msg->msg == 0x3FA) {
        static SS_GUI ss;
        ss.gui = msg->data0;
        ss.id = msg->submess;
        HookGUI(&ss);
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmpi(CFG_PATH, (char *)msg->data0) == 0) {
            InitConfig();
            ShowMSG(1, (int)"SieCrazySaver config updated!");
        }
    } else if (msg->msg == MSG_IPC) {
        IPC_REQ *ipc = (IPC_REQ*)msg->data0;
        if (strcmpi(ipc->name_to, ELF_NAME) == 0) {
            int csm_id = (int)ipc->data;
            if (csm_id != DAEMON_CSM_ID) {
                CloseCSM(csm_id);
            }
        }
    }
    return 1;
}

static void CSM_OnCreate(CSM_RAM *data) {
}

static void CSM_OnClose(CSM_RAM *data) {
    if (SS.id) {
        GeneralFunc_flag1(SS.id, 1);
    }
    SUBPROC((void *)kill_elf);
}

static const struct {
    CSM_DESC maincsm;
    WSHDR maincsm_name;
} MAINCSM = {
        {
                CSM_OnMessage,
                CSM_OnCreate,
#ifdef NEWSGOLD
                0,
                0,
                0,
                0,
#endif
                CSM_OnClose,
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
    wsprintf((WSHDR *)(&MAINCSM.maincsm_name), ELF_NAME);
}

int main() {
    CSM_RAM *save_cmpc;
    char dummy[sizeof(MAIN_CSM)];
    UpdateCSMname();
    InitConfig();
    LockSched();
    save_cmpc = CSM_root()->csm_q->current_msg_processing_csm;
    CSM_root()->csm_q->current_msg_processing_csm = CSM_root()->csm_q->csm.first;
    DAEMON_CSM_ID = CreateCSM(&MAINCSM.maincsm, dummy, 0);
    CSM_root()->csm_q->current_msg_processing_csm = save_cmpc;
    UnlockSched();
    // check double run
    static IPC_REQ ipc;
    ipc.name_to = ELF_NAME;
    ipc.name_from = ELF_NAME;
    ipc.data = (void*)DAEMON_CSM_ID;
    GBS_SendMessage(MMI_CEPID, MSG_IPC, 0, &ipc);
    return 0;
}
