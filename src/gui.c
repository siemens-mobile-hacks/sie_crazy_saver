#include <swilib.h>
#include <stdlib.h>
#include <sie/sie.h>
#include "gui.h"
#include "backlight.h"

extern char CFG_MP_CSM_ADDR[];
extern int CFG_ENABLE_MP_ILLUMINATION;
extern int CFG_FONT_SIZE_CLOCK, CFG_FONT_SIZE_TRACK, CFG_FONT_SIZE_CLOCK2;

extern unsigned int SS_CSM_ID;

unsigned int GUI_ID;
unsigned int CODE_PROTECTION_CSM_ID;

RECT canvas = { 0 };

GBSTMR TMR;
GBSTMR TMR_REDRAW;
GBSTMR TMR_ILLUMINATION = { 0 };

unsigned int COLOR_BG_ID;
unsigned int COLOR_TEXT_ID = 0;

unsigned int IsMPOn() {
    return (Sie_CSM_FindByAddr(CFG_MP_CSM_ADDR)) ? 1 : 0;
}

unsigned int IsCodeProtection() {
    int flag = 0;
    SettingsAE_GetFlag(&flag, SETTINGS_ID_APIDC_SETUP, "Screensaver", "CodeProtection");
    return flag;
}

void ChangeColors() {
    COLOR_BG_ID++;
    if (COLOR_BG_ID >= 23) {
        COLOR_BG_ID = 1;
    }
    if (COLOR_BG_ID <= 3 || COLOR_BG_ID == 14 || COLOR_BG_ID == 15) {
        COLOR_TEXT_ID = 0;
    } else {
        COLOR_TEXT_ID = 1;
    }
}

void Illumination_Proc() {
    if (CFG_ENABLE_MP_ILLUMINATION && IsMPOn() && !IsCalling()) {
        if (TMR_ILLUMINATION.param6 == 0) {
            BacklightOff();
            TMR_ILLUMINATION.param6 = 1;
            GBS_StartTimerProc(&TMR, 216 * 1 + 50, ChangeColors);
        } else {
            BacklightOn(100);
            TMR_ILLUMINATION.param6 = 0;
        }
    } else {
        COLOR_BG_ID = 1;
        COLOR_TEXT_ID = 0;
        if (TMR_ILLUMINATION.param6 == 0) { // была включена подсветка, нужно выключить
            BacklightOff();
            TMR_ILLUMINATION.param6 = 1;
        }
    }
    GBS_StartTimerProc(&TMR_ILLUMINATION, 216 * 2 + 100, Illumination_Proc);
}

void Redraw_Proc() {
    DirectRedrawGUI();
    GBS_StartTimerProc(&TMR_REDRAW, 216, Redraw_Proc);
}

void DrawBG(int x, int y, int x2, int y2) {
    unsigned int color_bg_id = 0;
    if (CFG_ENABLE_MP_ILLUMINATION && IsMPOn()) {
        color_bg_id = COLOR_BG_ID;
    } else {
        color_bg_id = 1;
    }
    DrawRectangle(0, 0, ScreenW() - 1, ScreenH() - 1, 0,
                  GetPaletteAdrByColorIndex((int)color_bg_id),
                  GetPaletteAdrByColorIndex((int)color_bg_id));
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

void OnRedraw(MAIN_GUI *data) {
    WSHDR *time_ws = GetTime();
    DrawBG(0, 0, ScreenW() - 1, ScreenH() - 1);
    if (IsMPOn()) {
        // track
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
            WSHDR *ws = AllocWS(256);
            unsigned int w, h;

            if (wstrlen(file_prop.tag_artist_ws) && wstrlen(file_prop.tag_title_ws)) {
                wsprintf(ws, "%w - %w", file_prop.tag_artist_ws, file_prop.tag_title_ws);
            } else {
                wstrcpy(ws, filename_ws);
            }

            Sie_FT_GetStringSize(time_ws, CFG_FONT_SIZE_CLOCK2, &w, &h);
            Sie_FT_DrawText(ws, 0, 0, ScreenW() - 1, ScreenH() - 1 - (int)h - 4,
                            CFG_FONT_SIZE_TRACK,
                            SIE_FT_TEXT_ALIGN_CENTER | SIE_FT_TEXT_VALIGN_MIDDLE,
                            GetPaletteAdrByColorIndex((int)COLOR_TEXT_ID));
            Sie_FT_DrawBoundingString(time_ws, 0, ScreenH() - 1 - (int)h, ScreenW() - 1, ScreenH() - 1,
                                      CFG_FONT_SIZE_CLOCK2, SIE_FT_TEXT_ALIGN_CENTER,
                                      GetPaletteAdrByColorIndex((int)COLOR_TEXT_ID));
            FreeWS(ws);
            FreeWS(file_prop.filename);
            FreeWS(file_prop.tag_title_ws);
            FreeWS(file_prop.tag_artist_ws);
        } else {
            FreeWS(file_prop.filename);
            FreeWS(file_prop.tag_title_ws);
            FreeWS(file_prop.tag_artist_ws);
            goto DRAW_CLOCK;
        }
    } else {
        DRAW_CLOCK:
        Sie_FT_DrawBoundingString(time_ws, 0, 0, ScreenW() - 1, ScreenH() - 1,
                                  CFG_FONT_SIZE_CLOCK,
                                  SIE_FT_TEXT_ALIGN_CENTER | SIE_FT_TEXT_VALIGN_MIDDLE,
                                  GetPaletteAdrByColorIndex((int)COLOR_TEXT_ID));
    }
    FreeWS(time_ws);
}

static void OnCreate(MAIN_GUI *data, void *(*malloc_adr)(int)) {
    data->gui.state = 1;
    COLOR_BG_ID = 1;
    COLOR_TEXT_ID = 0;
    BacklightOff();
}

static void OnClose(MAIN_GUI *data, void (*mfree_adr)(void *)) {
    GUI_ID = 0;
    data->gui.state = 0;
    GBS_DelTimer(&TMR);
    GBS_DelTimer(&TMR_REDRAW);
    GBS_DelTimer(&TMR_ILLUMINATION);
    CloseCSM((int)SS_CSM_ID);
    BacklightOnDefault();
}

void OnFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *)) {
    data->gui.state = 2;
    KbdLock();
    DisableIDLETMR();
#ifdef ELKA
    DisableIconBar(1);
#endif
    Redraw_Proc();
    GBS_StartTimerProc(&TMR_ILLUMINATION, 216 * 2, Illumination_Proc);
}

static void OnUnFocus(MAIN_GUI *data, void (*mfree_adr)(void *)) {
    if (data->gui.state != 2) return;
    data->gui.state = 1;
#ifdef ELKA
    DisableIconBar(0);
#endif
    GBS_DelTimer(&TMR);
    GBS_DelTimer(&TMR_REDRAW);
    GBS_DelTimer(&TMR_ILLUMINATION);
}

static int OnKey(MAIN_GUI *data, GUI_MSG *msg) {
    if (msg->gbsmsg->msg == LONG_PRESS) {
        if (msg->gbsmsg->submess == '#') {
            if (IsCodeProtection()) {
//                *((char*)GetScreenSaverRAM() + 0x38) = -1; // bypass default code protection
                CODE_PROTECTION_CSM_ID = ShowScreenSaverCodeProtection();
            } else {
                KbdUnlock();
                CloseScreensaver();
            }
        }
    } else if (msg->gbsmsg->msg == KEY_UP) {
        if (!CFG_ENABLE_MP_ILLUMINATION || !IsMPOn()) {
            BacklightOn(100);
            GBS_StartTimerProc(&TMR, 216 * 1, BacklightOff);
        }
    }
    return 0;
}

static int method8(void) { return 0; }
static int method9(void) { return 0; }

const void *const gui_methods[11] = {
        (void*)OnRedraw,
        (void*)OnCreate,
        (void*)OnClose,
        (void*)OnFocus,
        (void*)OnUnFocus,
        (void*)OnKey,
        0,
        (void*)kill_data,
        (void*)method8,
        (void*)method9,
        0
};

MAIN_GUI *CreateCrazyGUI() {
    MAIN_GUI *main_gui = malloc(sizeof(MAIN_GUI));
    zeromem(main_gui, sizeof(MAIN_GUI));
    Sie_GUI_InitCanvas(&canvas);
    main_gui->gui.canvas = (RECT*)(&canvas);
    main_gui->gui.methods = (void*)gui_methods;
    main_gui->gui.item_ll.data_mfree = (void (*)(void *))mfree_adr();
    GUI_ID = CreateGUI(main_gui);
    return main_gui;
}
