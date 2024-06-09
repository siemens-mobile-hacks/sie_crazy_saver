#pragma once

typedef struct {
    GUI gui;
    int timer_id;
    int redraw_timer_id;

    int color_bg_id;
    int color_text_id;
} MAIN_GUI;

MAIN_GUI *CreateCrazyGUI();
