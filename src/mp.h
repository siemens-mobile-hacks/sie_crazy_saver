#pragma once

typedef struct  {
    CSM_RAM csm_ram;
    WSHDR *filename_ws;
    WSHDR *dir_ws;
} CSM_RAM_MP;

unsigned int IsMPOn();
CSM_RAM_MP *FindCSMMediaPlayer();
int GetTrack(WSHDR *track, CSM_RAM_MP *csm);
