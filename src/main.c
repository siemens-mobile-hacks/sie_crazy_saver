#include <swilib.h>
#include <string.h>
#include <sie/sie.h>
#include "csm.h"
#include "config_loader.h"

#define ELF_NAME "SieCrazySaver"

extern char CFG_PATH[];
extern unsigned int CSM_ID;

unsigned int SS_CSM_ID;
unsigned int DAEMON_CSM_ID;

const int minus11 =- 11;
unsigned short maincsm_name_body[140];

CSM_DESC *old_csmd;
CSM_DESC new_csmd;
//int (*(old_onmessage))(CSM_RAM *csm, GBS_MSG *msg);
void (*(old_onclose))(CSM_RAM *csm);

typedef struct {
    CSM_RAM csm;
    CSM_RAM *csm_ss;
} MAIN_CSM;

//int ss_csm_onmessage(CSM_RAM *csm, GBS_MSG *msg) {
//    int result = old_onmessage(csm, msg);
//    return result;
//}
//
void ss_csm_onclose(CSM_RAM *csm) {
    CloseCSM((int)CSM_ID);
    old_onclose(csm);
    old_csmd = NULL;
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (msg->msg == 0xA000 && GetScreenSaverType() == SCREENSAVER_ENERGY_SAVER) {
        SS_CSM_ID = msg->submess;
        csm->csm_ss = FindCSMbyID((int)SS_CSM_ID);
        if (csm->csm_ss) {
            LockSched();
            old_csmd = csm->csm_ss->constr;
//            old_onmessage = csm_ss->constr->onMessage;
            old_onclose = csm->csm_ss->constr->onClose;
            memcpy(&new_csmd, old_csmd, sizeof(CSM_DESC));
//            new_csmd.onMessage = ss_csm_onmessage;
            new_csmd.onClose = ss_csm_onclose;
            csm->csm_ss->constr = &new_csmd;
            UnlockSched();
            Sie_SubProc_Run(CreateCrazyCSM, NULL);
        }
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmpi(CFG_PATH, (char *)msg->data0) == 0) {
            ShowMSG(1, (int)"SieCrazySaver config updated!");
            InitConfig();
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

static void maincsm_oncreate(CSM_RAM *data) {
}

static void maincsm_onclose(CSM_RAM *data) {
    MAIN_CSM *csm = (MAIN_CSM*)data;
    if (old_csmd) {
        csm->csm_ss->constr = old_csmd;
    }
    CloseCSM((int)CSM_ID);
    SUBPROC((void *)kill_elf);
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
    DAEMON_CSM_ID = CreateCSM(&MAINCSM.maincsm,dummy,0);
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
