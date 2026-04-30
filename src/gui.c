#include <swilib.h>
#include "mp.h"
#include "gui.h"
#include "config.h"
#include "keyhook.h"

typedef struct {
    int timer_id;
    int redraw_timer_id;
    int illumination_timer_id;
    int mp_flag;
    int illumination_flag;
    int brightness;
    int color_bg_id;
    int color_text_id;
} GUI_DATA;

extern SS_GUI SS;
extern GUI_METHODS *METHODS_OLD;

RECT canvas = { 0 };
GUI_DATA DATA;

void InitData() {
    zeromem(&DATA, sizeof(GUI_DATA));
    DATA.color_bg_id = 1;
}

int GetSystemBrightness() {
    int brightness = 100;
    if (SettingsAE_Read(&brightness, SETTINGS_ID_SETUP, 0x0, "DISPLAY_ILLUMINATION") == 0) {
        if (brightness > 100) {
            brightness = 100;
        } else if (brightness < 0) {
            brightness = 0;
        }
    }
    return brightness;
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
    int brightness = DATA.brightness;
    if (IsMPOn() && CFG.enable_illumination) {
        DATA.mp_flag = 1;
        if (CFG.override_brightness) {
            brightness = 100;
        }
        SetMaxIllumIntensity(3, brightness);
        if (DATA.illumination_flag == 0) {
            DATA.illumination_flag = 1;
            DirectRedrawGUI_ID(SS.id);
            TempLightOn(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 0x7FFF);
        } else {
            IllumFilterSet(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 0x7FFF);
            DATA.illumination_flag = 0;
            GUI_StartTimerProc(gui, DATA.timer_id, 1000, ChangeColors);
        }
    } else if (DATA.mp_flag == 1) {
        DATA.mp_flag = 0;
        DirectRedrawGUI_ID(SS.id);
        SetMaxIllumIntensity(3, DATA.brightness);
        IllumFilterSet(SET_LIGHT_DISPLAY | SET_LIGHT_KEYBOARD, 1);
    }
    GUI_StartTimerProc(gui, DATA.illumination_timer_id, 2000, IlluminationProc);
}

void Redraw_Proc(void *gui) {
    if (!IsMPOn() || !CFG.enable_illumination) {
        DATA.color_bg_id = 1;
        DATA.color_text_id = 0;
        DirectRedrawGUI_ID(SS.id);
    }
    GUI_StartTimerProc(gui, DATA.redraw_timer_id, 1000, Redraw_Proc);
}

WSHDR *GetDate(const TDate *date) {
    WSHDR *ws = AllocWS(32);
    GetDate_ws(ws, date, 0x7);
    return ws;
}

WSHDR *GetTime(const TTime *time) {
    size_t len = 0;
    WSHDR *ws = AllocWS(32);

    GetTime_ws(ws, time, 0x223);
    len = wstrlen(ws);
    if (len > 5) { // cut am, pm
        wsRemoveChars(ws, 5 + 1, (int)len);
    }
    return ws;
}

void DrawDigitalClock(const TTime *time) {
    int hour = time->hour;
    if (RamDateTimeSettings()->timeFormat == 1) { // 12
        hour = hour % 12;
        if (hour == 0) {
            hour = 12;
        }
    }

    const int icon_digit_0 = CFG.icon_digit_0;
    const int icon_colon = icon_digit_0 + 11;
    const int digit_h = GetImgHeight(icon_digit_0);
    const int digit_w = GetImgWidth(icon_digit_0);
    const int digit_space = 10;
    const int colon_w = GetImgWidth(icon_colon);
    const int colon_space = 6;
    const int clock_w = digit_w * 4 + colon_w + colon_space * 2 + digit_space * 2;

    int x = (ScreenW() - clock_w) / 2;
    int y = (ScreenH() - digit_h) / 2;
    DrawImg(x, y, icon_digit_0 + (hour / 10));
    x += digit_w + digit_space;
    DrawImg(x, y, icon_digit_0 + (hour % 10));
    x += digit_w + colon_space;
    DrawImg(x, y, icon_colon);
    x += colon_w + colon_space;
    DrawImg(x, y, icon_digit_0 + (time->min / 10));
    x += digit_w + digit_space;
    DrawImg(x, y, icon_digit_0 + (time->min % 10));
}

#define GetStringSize ((void (*)(WSHDR *, int text_flags, int flags, int font, int *w, int *h))(0xa08d32c4 | 1))

void OnRedraw(GUI *gui) {
    METHODS_OLD->onRedraw(gui);
    TDate date;
    TTime time;
    GetDateTime(&date, &time);
    DrawRectangle(0, 0, ScreenW() - 1, ScreenH() - 1, 0,
                  GetPaletteAdrByColorIndex((int)DATA.color_bg_id),
                  GetPaletteAdrByColorIndex((int)DATA.color_bg_id));
    MP_CSM *csm = IsMPOn();
    if (csm) {
        WSHDR *track = AllocWS(256);
        if (GetTrack(track, csm)) {
            WSHDR *clock_ws = AllocWS(64);
            WSHDR *date_ws = GetDate(&date);
            WSHDR *time_ws = GetTime(&time);
            wsprintf(clock_ws, "%w %w", date_ws, time_ws);
            FreeWS(time_ws);
            FreeWS(date_ws);

            int font = FONT_MEDIUM;
            const int clock_w = Get_WS_width(clock_ws, font);
            const int clock_h = GetFontYSIZE(font);
            int x = (ScreenW() - 1 - clock_w) / 2;
            int y = (ScreenH() - 1 - clock_h);
            DrawString(clock_ws, x, y, x + clock_w, y + clock_h, font, TEXT_ALIGNMIDDLE,
                GetPaletteAdrByColorIndex(DATA.color_text_id), GetPaletteAdrByColorIndex(0x17));
            FreeWS(clock_ws);

            font = FONT_MEDIUM;
            int track_w = ScreenW() - 1;
            int track_h = ScreenH() - 1 - clock_h;
            Get_WS_extent(track, TEXT_ALIGNMIDDLE, 0, font, &track_w, &track_h);
            x = (ScreenW() - 1 - track_w) / 2;
            y = (ScreenH() - 1 - track_h) / 2;
            DrawString(track, x, y, x + track_w, y + track_h, font, TEXT_ALIGNMIDDLE,
                GetPaletteAdrByColorIndex(DATA.color_text_id), GetPaletteAdrByColorIndex(0x17));
            FreeWS(track);
        } else {
            FreeWS(track);
            goto DRAW_CLOCK;
        }
    } else {
        DRAW_CLOCK:
            DrawDigitalClock(&time);
            WSHDR *date_ws = GetDate(&date);
            int font = FONT_MEDIUM;
            const int x = 0;
            const int y = ScreenH() - 1 - GetFontYSIZE(font);
            const int x2 = ScreenW() - 1;
            const int y2 = ScreenH() - 1;
            DrawString(date_ws, x, y, x2, y2, font, TEXT_ALIGNMIDDLE,
                GetPaletteAdrByColorIndex(DATA.color_text_id), GetPaletteAdrByColorIndex(0x17));
    }
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
    DATA.brightness = GetSystemBrightness();
    if (IsMPOn() && CFG.enable_illumination) {
        GUI_StartTimerProc(gui, DATA.timer_id, 1000, ChangeColors);
    } else {
        DATA.color_bg_id = 1;
        DATA.color_text_id = 0;
    }
    if (IsMPOn() && CFG.enable_illumination) {
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
    SetMaxIllumIntensity(3, DATA.brightness);
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
