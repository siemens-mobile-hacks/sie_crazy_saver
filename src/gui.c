#include <swilib.h>
#include <stdlib.h>
#include <sie/sie.h>
#include "gui.h"

extern unsigned int SS_CSM_ID;

RECT canvas = { 0 };

GBSTMR TMR;
GBSTMR TMR_REDRAW;
GBSTMR TMR_ILLUMINATION = { 0 };

unsigned int COLOR_BG_ID;
unsigned int COLOR_TEXT_ID = 0;

void BacklightOn() {
    SetIllumination(ILLUMINATION_DEV_KEYBOARD, 1, 100, 1000);
    SetIllumination(ILLUMINATION_DEV_DISPLAY, 1, 100, 1000);
}

void BacklightOff() {
    SetIllumination(ILLUMINATION_DEV_KEYBOARD, 1, 0, 1000);
    SetIllumination(ILLUMINATION_DEV_DISPLAY, 1, 0, 1000);
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
    if (IsPlayerOn() && !IsCalling()) {
        if (TMR_ILLUMINATION.param6 == 0) {
            BacklightOff();
            TMR_ILLUMINATION.param6 = 1;
            GBS_StartTimerProc(&TMR, 216 * 1 + 50, ChangeColors);
        } else {
            BacklightOn();
            TMR_ILLUMINATION.param6 = 0;
        }
    } else {
        COLOR_BG_ID = 1;
    }
    GBS_StartTimerProc(&TMR_ILLUMINATION, 216 * 2 + 100, Illumination_Proc);
}

void Redraw_Proc() {
    DirectRedrawGUI();
    GBS_StartTimerProc(&TMR_REDRAW, 216, Redraw_Proc);
}

void DrawBG(int x, int y, int x2, int y2) {
    unsigned int color_bg_id = 0;
    if (IsPlayerOn()) {
        color_bg_id = COLOR_BG_ID;
    } else {
        color_bg_id = 1;
    }
    DrawRectangle(0, 0, ScreenW() - 1, ScreenH() - 1, 0,
                  GetPaletteAdrByColorIndex((int)color_bg_id),
                  GetPaletteAdrByColorIndex((int)color_bg_id));
}

void OnRedraw(MAIN_GUI *data) {
    DrawBG(0, 0, ScreenW() - 1, ScreenH() - 1);
    if (IsPlayerOn()) {
        // track
        WSHDR *dir_ws = (WSHDR*)GetLastAudioTrackDir();
        WSHDR *filename_ws = (WSHDR*)GetLastAudioTrackFilename();

        FILE_PROP file_prop = { 0 };
        file_prop.type = FILE_PROP_TYPE_MUSIC;
        file_prop.filename = AllocWS(256);
        file_prop.tag_title_ws = AllocWS(64);
        file_prop.tag_artist_ws = AllocWS(64);

        wsprintf(file_prop.filename, "%w\\%w", dir_ws, filename_ws);
        if (GetFileProp(&file_prop, filename_ws, dir_ws)) {
            WSHDR *ws = AllocWS(256);
            if (wstrlen(file_prop.tag_artist_ws) && wstrlen(file_prop.tag_title_ws)) {
                wsprintf(ws, "%w - %w", file_prop.tag_artist_ws, file_prop.tag_title_ws);
            } else {
                wstrcpy(ws, filename_ws);
            }
            Sie_FT_DrawText(ws, 0, 0, ScreenW() - 1, ScreenH() - 1,
                            18,SIE_FT_TEXT_ALIGN_CENTER | SIE_FT_TEXT_VALIGN_MIDDLE,
                            GetPaletteAdrByColorIndex((int)COLOR_TEXT_ID));
            FreeWS(ws);
        }
        FreeWS(file_prop.filename);
        FreeWS(file_prop.tag_title_ws);
        FreeWS(file_prop.tag_artist_ws);
    } else {
        TTime time; TDate date;
        GetDateTime(&date, &time);
        WSHDR *ws = AllocWS(64);
        wsprintf(ws, "%02d:%02d", time.hour, time.min);
        Sie_FT_DrawBoundingString(ws, 0, 0, ScreenW() - 1, ScreenH() - 1, 52,
                                  SIE_FT_TEXT_ALIGN_CENTER | SIE_FT_TEXT_VALIGN_MIDDLE,
                                  GetPaletteAdrByColorIndex((int)COLOR_TEXT_ID));
    }
}

static void OnCreate(MAIN_GUI *data, void *(*malloc_adr)(int)) {
    data->gui.state = 1;
    COLOR_BG_ID = 1;
    COLOR_TEXT_ID = 0;
    BacklightOff();
}

static void OnClose(MAIN_GUI *data, void (*mfree_adr)(void *)) {
    data->gui.state = 0;
    CloseCSM((int)SS_CSM_ID);
}

void OnFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *)) {
    data->gui.state = 2;
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
            KbdUnlock();
            BacklightOn();
            CloseScreensaver();
        }
    } else if (msg->gbsmsg->msg == KEY_UP) {
        if (!IsPlayerOn()) {
            BacklightOn();
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
    main_gui->gui_id = CreateGUI(main_gui);
    return main_gui;
}
