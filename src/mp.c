#include <swilib.h>
#include <stdlib.h>
#include "mp.h"
#include "config.h"

CSM_RAM *FindCSMByConstr(CSM_RAM *csm, const void *constr) {
    CSM_RAM *csm_ram = NULL;
    LockSched();
    while (csm) {
        if (csm->constr == constr) {
            csm_ram = csm;
            break;
        }
        csm = csm->next;
    }
    UnlockSched();
    return csm_ram;
}

CSM_RAM *FindCSMByAddr(const char *addr) {
    const void *constr = (void*)strtoul(addr, NULL, 16);
    CSM_RAM *csm = FindCSMByConstr(CSM_root()->csm_q->csm.first, constr);
#ifdef NEWSGOLD
    if (!csm) {
        csm = FindCSMByConstr(CSM_root()->csm_q->csm_background.first, constr);
    }
#endif
    return csm;
}

MP_CSM *IsMPOn() {
    MP_CSM *csm = (MP_CSM*)FindCSMByAddr(CFG.mp_csm_addr);
    if (csm) {
#ifdef NEWSGOLD
#ifndef ELKA
        const enum Accessory ims_700[] = {ACC_MOBILE_MUSIC_SET};
        if (CFG.detect_ims_700 && !IsAnyOfAccessoriesConnected(ims_700, 1)) {
            csm = NULL;
        }
#endif
#endif
    }
    return csm;
}

int GetTrack(WSHDR *track, const MP_CSM *csm) {
    int result = 0;
#ifdef NEWSGOLD
    FILE_PROP file_prop = { 0 };
    file_prop.type = FILE_PROP_TYPE_MUSIC;
    file_prop.filename = AllocWS(256);
    file_prop.tag_title_ws = AllocWS(64);
    file_prop.tag_artist_ws = AllocWS(64);

    wstrcpy(file_prop.filename, csm->dir_ws);
    if (wsCharAt(file_prop.filename, (short)wstrlen(file_prop.filename)) != '\\') {
        wsAppendChar(file_prop.filename, '\\');
    }
    wstrcat(file_prop.filename, csm->filename_ws);

    if (GetFileProp(&file_prop, csm->filename_ws, csm->dir_ws)) {
        if (wstrlen(file_prop.tag_artist_ws) && wstrlen(file_prop.tag_title_ws)) {
            wsprintf(track, "%w - %w", file_prop.tag_artist_ws, file_prop.tag_title_ws);
        } else {
            wstrcpy(track, csm->filename_ws);
        }
        result = 1;
    }
    FreeWS(file_prop.filename);
    FreeWS(file_prop.tag_title_ws);
    FreeWS(file_prop.tag_artist_ws);
#else
    WSHDR *dir_ws = csm->dir_ws;
    WSHDR *filename_ws = csm->filename_ws;
    FILE_PROP file_prop = { 0 };
    file_prop.type = FILE_PROP_TYPE_AUDIO;
    file_prop.filename = AllocWS(256);
    wstrcpy(file_prop.filename, dir_ws);
    if (wsCharAt(file_prop.filename, (short)wstrlen(file_prop.filename)) != '\\') {
        wsAppendChar(file_prop.filename, '\\');
    }
    wstrcat(file_prop.filename, filename_ws);
    if (GetFileProp(&file_prop, filename_ws, dir_ws)) {
        wstrcpy(track, filename_ws);
        result = 1;
    }
    FreeWS(file_prop.filename);
#endif
    return result;
}
