#pragma once

typedef struct  {
    CSM_RAM csm_ram;
#ifdef NEWSGOLD
    int unk;
#endif
    WSHDR *filename_ws;
    WSHDR *dir_ws;
} MP_CSM;

MP_CSM *IsMPOn();
int GetTrack(WSHDR *track, const MP_CSM *csm);
