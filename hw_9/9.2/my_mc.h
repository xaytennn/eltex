#ifndef MY_MC_H
#define MY_MC_H

#include <dirent.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Panel{
    WINDOW *wnd;
    struct dirent **namelist;
    int n;
    int current_selection;
    char path[1024];
} Panel;

void draw_namelist(Panel *panel, int tab);
void load_panel(Panel *panel, const char *path);
void enter_directory(Panel *panel);

#endif
