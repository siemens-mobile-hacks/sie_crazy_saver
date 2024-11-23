#include <sie/sie.h>
#include "mp.h"

CSM_RAM_MP *FindCSMMediaPlayer() {
    extern char CFG_MP_CSM_ADDR[];
    return (CSM_RAM_MP*)Sie_CSM_FindByAddr(CFG_MP_CSM_ADDR);
}

unsigned int IsMPOn() {
    return FindCSMMediaPlayer() ? 1 : 0;
}

int GetTrack(WSHDR *track, CSM_RAM_MP *csm) {
    int result = 0;
#ifdef NEWSGOLD
    WSHDR *dir_ws = (WSHDR*)GetLastAudioTrackDir();
    WSHDR *filename_ws = (WSHDR*)GetLastAudioTrackFilename();

    FILE_PROP file_prop = { 0 };
    file_prop.type = FILE_PROP_TYPE_MUSIC;
    file_prop.filename = AllocWS(256);
    file_prop.tag_title_ws = AllocWS(64);
    file_prop.tag_artist_ws = AllocWS(64);

    wstrcpy(file_prop.filename, dir_ws);
    if (wsCharAt(file_prop.filename, (short)wstrlen(file_prop.filename)) != '\\') {
        wsAppendChar(file_prop.filename, '\\');
    }
    wstrcat(file_prop.filename, filename_ws);

    if (GetFileProp(&file_prop, filename_ws, dir_ws)) {
        if (wstrlen(file_prop.tag_artist_ws) && wstrlen(file_prop.tag_title_ws)) {
            wsprintf(track, "%w - %w", file_prop.tag_artist_ws, file_prop.tag_title_ws);
        } else {
            wstrcpy(track, filename_ws);
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
