#pragma once

typedef struct  {
    CSM_RAM csm_ram;
    WSHDR *filename_ws;
    WSHDR *dir_ws;
} CSM_RAM_MP;

CSM_RAM_MP *IsMPOn();
int GetTrack(WSHDR *track, CSM_RAM_MP *csm);
