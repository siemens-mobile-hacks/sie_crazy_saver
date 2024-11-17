#pragma once

typedef struct {
    GUI *gui;
    int id;
} SS_GUI;

void InitData();
void OnRedraw(GUI *gui);
void OnClose(GUI *gui, void (*mfree_adr)(void *));
void OnFocus(GUI *gui, void *(*malloc_adr)(size_t), void (*mfree_adr)(void *));
void OnUnFocus(GUI *gui, void (*mfree_adr)(void *));
int OnKey(GUI *gui, GUI_MSG *msg);
void OnDestroy(void *gui, void (*mfree_adr)(void *));
