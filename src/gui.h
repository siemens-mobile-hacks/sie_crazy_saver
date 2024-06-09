#pragma once

typedef struct {
    GUI gui;
    int timer_id;
    int redraw_timer_id;
    int illumination_flag;
    int illumination_timer_id;
} MAIN_GUI;

MAIN_GUI *CreateCrazyGUI();
