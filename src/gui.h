#pragma once

typedef struct {
    GUI gui;
    unsigned int gui_id;
} MAIN_GUI;

void BacklightOn();
void BacklightOff();

MAIN_GUI *CreateCrazyGUI();
void CloseCrazyGUI();

