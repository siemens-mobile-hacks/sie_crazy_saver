#include <swilib.h>
#include <stdlib.h>
#include <sie/sie.h>
#include "mp.h"
#include "gui.h"
#include "keyhook.h"

typedef struct {
    int timer_id;
    int redraw_timer_id;
    int illumination_timer_id;
    int illumination_flag;
    int color_bg_id;
    int color_text_id;
} GUI_DATA;

extern int CFG_ENABLE_ILLUMINATION;
extern int CFG_FONT_SIZE_CLOCK, CFG_FONT_SIZE_TRACK, CFG_FONT_SIZE_CLOCK2;
extern SS_GUI SS;
extern GUI_METHODS *METHODS_OLD;

RECT canvas = { 0 };
GUI_DATA DATA;

void InitData() {
    zeromem(&DATA, sizeof(GUI_DATA));
    DATA.color_bg_id = 1;
}

void DeleteTimers(void *gui) {
    if (DATA.timer_id) {
        GUI_DeleteTimer(gui, DATA.timer_id);
    }
    if (DATA.redraw_timer_id) {
        GUI_DeleteTimer(gui, DATA.redraw_timer_id);
    }
    if (DATA.illumination_timer_id) {
        GUI_DeleteTimer(gui, DATA.illumination_timer_id);
    }
    DATA.timer_id = 0;
    DATA.redraw_timer_id = 0;
    DATA.illumination_timer_id = 0;
}

void ChangeColors(void *gui) {
    DATA.color_bg_id++;
    if (DATA.color_bg_id >= 23) {
        DATA.color_bg_id = 1;
    }
    if (DATA.color_bg_id <= 3 || DATA.color_bg_id == 14 || DATA.color_bg_id == 15) {
        DATA.color_text_id = 0;
    } else {
        DATA.color_text_id = 1;
    }
    DirectRedrawGUI_ID(SS.id);
}

void IlluminationProc(void *gui) {
    if (IsMPOn() && CFG_ENABLE_ILLUMINATION) {
        if (DATA.illumination_flag == 0) {
            TempLightOn(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 0x7FFF);
            DATA.illumination_flag = 1;
        } else {
            IllumFilterSet(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 1);
            DATA.illumination_flag = 0;
            GUI_StartTimerProc(gui, DATA.timer_id, 1000, ChangeColors);
        }
    }
    GUI_StartTimerProc(gui, DATA.illumination_timer_id, 2000, IlluminationProc);
}

void Redraw_Proc(void *gui) {
    if (!IsMPOn() || !CFG_ENABLE_ILLUMINATION) {
        DATA.color_bg_id = 1;
        DATA.color_text_id = 0;
        DirectRedrawGUI_ID(SS.id);
    }
    GUI_StartTimerProc(gui, DATA.redraw_timer_id, 1000, Redraw_Proc);
}

WSHDR *GetTime() {
    TTime time;
    unsigned int len = 0;
    WSHDR *ws = AllocWS(32);

    GetDateTime(NULL, &time);
    GetTime_ws(ws, &time, 0x223);
    len = wstrlen(ws);
    if (len > 5) { // cut am, pm
        wsRemoveChars(ws, 5 + 1, (int)len);
    }
    return ws;
}

void OnRedraw(GUI *gui) {
    METHODS_OLD->onRedraw(gui);
    WSHDR *time_ws = GetTime();
    DrawRectangle(0, 0, ScreenW() - 1, ScreenH() - 1, 0,
                  GetPaletteAdrByColorIndex((int)DATA.color_bg_id),
                  GetPaletteAdrByColorIndex((int)DATA.color_bg_id));

    CSM_RAM_MP *csm = FindCSMMediaPlayer();
    if (csm) {
        WSHDR *track = AllocWS(256);
        if (GetTrack(track, csm)) {
            unsigned int w;
            unsigned int h;
            Sie_FT_GetStringSize(time_ws, CFG_FONT_SIZE_CLOCK2, &w, &h);
            Sie_FT_DrawText(track, 0, 0, ScreenW() - 1, ScreenH() - 1 - (int)h - 4,
                            CFG_FONT_SIZE_TRACK,
                            SIE_FT_TEXT_ALIGN_CENTER | SIE_FT_TEXT_VALIGN_MIDDLE,
                            GetPaletteAdrByColorIndex((int)DATA.color_text_id));
            Sie_FT_DrawBoundingString(time_ws, 0, ScreenH() - 1 - (int)h, ScreenW() - 1, ScreenH() - 1,
                                      CFG_FONT_SIZE_CLOCK2, SIE_FT_TEXT_ALIGN_CENTER,
                                      GetPaletteAdrByColorIndex((int)DATA.color_text_id));
            FreeWS(track);
        } else {
            FreeWS(track);
            goto DRAW_CLOCK;
        }
    } else {
        DRAW_CLOCK:
            Sie_FT_DrawBoundingString(time_ws, 0, 0, ScreenW() - 1, ScreenH() - 1,
                CFG_FONT_SIZE_CLOCK, SIE_FT_TEXT_ALIGN_CENTER | SIE_FT_TEXT_VALIGN_MIDDLE, GetPaletteAdrByColorIndex((int)DATA.color_text_id));
    }
    FreeWS(time_ws);
}

void Create(GUI *gui) {
    AddKeyHook();
}

void OnClose(GUI *gui, void (*mfree_adr)(void *)) {
    TempLightOn(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 0x7FFF);
    DeleteTimers(gui);
    RemoveKeyHook();
    METHODS_OLD->onClose(gui, mfree_adr);
    SS.id = 0;
}

void Focus(GUI *gui) {
    DisableIDLETMR();
#ifdef ELKA
    DisableIconBar(1);
#endif
    DATA.timer_id = GUI_NewTimer(gui);
    DATA.redraw_timer_id = GUI_NewTimer(gui);
    DATA.illumination_timer_id = GUI_NewTimer(gui);
    if (IsMPOn() && CFG_ENABLE_ILLUMINATION) {
        GUI_StartTimerProc(gui, DATA.timer_id, 1000, ChangeColors);
    } else {
        DATA.color_bg_id = 1;
        DATA.color_text_id = 0;
    }
    if (IsMPOn() && CFG_ENABLE_ILLUMINATION) {
        IllumFilterSet(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 1);
    }
    GUI_StartTimerProc(gui, DATA.redraw_timer_id, 1000, Redraw_Proc);
    GUI_StartTimerProc(gui, DATA.illumination_timer_id, 1000, IlluminationProc);
}

void OnFocus(GUI *gui, void *(*malloc_adr)(size_t), void (*mfree_adr)(void *)) {
    METHODS_OLD->onFocus(gui, malloc_adr, mfree_adr);
    Focus(gui);
}

void OnUnFocus(GUI *gui, void (*mfree_adr)(void *)) {
    DeleteTimers(gui);
#ifdef ELKA
    DisableIconBar(0);
#endif
    METHODS_OLD->onUnfocus(gui, mfree_adr);
}

void OnDestroy(void *gui, void (*mfree_adr)(void *)) {
    LockSched();
    SS.gui->methods = METHODS_OLD;
    UnlockSched();
    METHODS_OLD->onDestroy(gui, mfree_adr);
}
