#include "my_mc.h"

int filter_dot(const struct dirent *d) {
    return strcmp(d->d_name, ".") != 0;
}

void load_panel(Panel *panel, const char *path) {
    if (panel == NULL || path == NULL ) {
        fprintf(stderr, "Передана пустая панель или путь\n");
        return;
    }
    
    // Освобождаем память старого namelist, которую выделяет scandir()
    if (panel->namelist) {
        for (int i = 0; i < panel->n; i++) {
            free(panel->namelist[i]);
        }
        free(panel->namelist);
        panel->namelist = NULL;
    }
    
    strcpy(panel->path, path);
    panel->n = scandir(panel->path, &panel->namelist, filter_dot, alphasort);
    if (panel->n == -1) {
        perror("Ошибка вызова scandir\n");
        return;
    }
    panel->current_selection = 0;
}
