#include <swilib.h>
#include <string.h>
#include <sie/sie.h>
#include "csm.h"
#include "config_loader.h"

extern GBSTMR TMR;
extern char CFG_PATH[];
extern unsigned int CSM_ID;

unsigned int SS_CSM_ID;
const int minus11 =- 11;
unsigned short maincsm_name_body[140];

CSM_RAM *csm_ss;
CSM_DESC *old_csmd;
CSM_DESC new_csmd;
int (*(old_onmessage))(CSM_RAM *csm, GBS_MSG *msg);
void (*(old_onclose))(CSM_RAM *csm);

typedef struct {
    CSM_RAM csm;
} MAIN_CSM;

int ss_csm_onmessage(CSM_RAM *csm, GBS_MSG *msg) {
    int result = old_onmessage(csm, msg);
    return result;
}

void ss_csm_onclose(CSM_RAM *csm) {
    CloseCSM((int)CSM_ID);
    old_onclose(csm);
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg) {
    if (msg->msg == 0xA000) {
        SS_CSM_ID = msg->submess;
        csm_ss = FindCSMbyID((int)SS_CSM_ID);
        if (csm_ss) {
            LockSched();
            old_csmd = csm_ss->constr;
            old_onmessage = csm_ss->constr->onMessage;
            old_onclose = csm_ss->constr->onClose;
            memcpy(&new_csmd, old_csmd, sizeof(CSM_DESC));
            new_csmd.onMessage = ss_csm_onmessage;
            new_csmd.onClose = ss_csm_onclose;
            csm_ss->constr = &new_csmd;
            UnlockSched();
            Sie_SubProc_Run(CreateCrazyCSM, NULL);
        }
    } else if (msg->msg == MSG_RECONFIGURE_REQ) {
        if (strcmpi(CFG_PATH, (char *)msg->data0) == 0) {
            ShowMSG(1, (int)"SieCrazySaver config updated!");
            InitConfig();
        }
    }
    return 1;
}

static void maincsm_oncreate(CSM_RAM *data) {
}

static void maincsm_onclose(CSM_RAM *csm) {
    csm_ss->constr = old_csmd;
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
    wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"SieCrazySaver");
}

int main() {
    CSM_RAM *save_cmpc;
    char dummy[sizeof(MAIN_CSM)];
    UpdateCSMname();
    InitConfig();
    LockSched();
    save_cmpc = CSM_root()->csm_q->current_msg_processing_csm;
    CSM_root()->csm_q->current_msg_processing_csm = CSM_root()->csm_q->csm.first;
    CreateCSM(&MAINCSM.maincsm,dummy,0);
    CSM_root()->csm_q->current_msg_processing_csm = save_cmpc;
    UnlockSched();
    return 0;
}
