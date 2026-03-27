#include "my_mc.h"


void draw_namelist(Panel *panel, int tab){
    if (panel == NULL || panel->wnd == NULL ) {
        fprintf(stderr, "Передана пустая панель или не создано окно\n");
        return;
    }
    if (panel->namelist == NULL || panel->n <= 0) {
        fprintf(stderr, "Пустой список файлов и каталогов\n");
        return;
    }


    int height, width;
    getmaxyx(panel->wnd, height, width);

    werase(panel->wnd);
    box(panel->wnd, 0, 0);
    
    wattron(panel->wnd, A_REVERSE);
    mvwprintw(panel->wnd, 0, 2, " %s ", panel->path);
    wattroff(panel->wnd, A_REVERSE);
    
    for (int i = 0; i < panel->n && i < height - 2; i++) {
        
        if (i == panel->current_selection && tab) {
            wattron(panel->wnd, A_REVERSE);
            mvwprintw(panel->wnd, i + 1, 2, "%s", panel->namelist[i]->d_name);
            wattroff(panel->wnd, A_REVERSE);
        } else {
            mvwprintw(panel->wnd, i + 1, 2, "%s", panel->namelist[i]->d_name);
        }
    }
    
    wrefresh(panel->wnd);
}
