#include "my_mc.h"

void enter_directory(Panel *panel){
    if (panel == NULL || panel->namelist == NULL ) {
        fprintf(stderr, "Передана пустая панель или список каталогов и файлов\n");
        return;
    }

    if (panel->n == 0 || panel->current_selection >= panel->n) {
        return;
    }
    
        
    struct dirent *newdir = panel->namelist[panel->current_selection];
    if (newdir == NULL) {
        fprintf(stderr, "Ошибка создании новой структуры dirent\n");        
        return;
    }    
    
    char new_path[1024];
    int len = 0;
    
    // Проверка, что нажато ".." и выход на уровень выше
    if (strcmp(newdir->d_name, "..") == 0) {
    
    // Ищем последний '/'
        char *last_slash = strrchr(panel->path, '/');
        
        // Если мы в корне, то остаемся здесь
        if (last_slash == panel->path) {
            strcpy(new_path, "/");
        } else {
        
        // Если есть директории выше, то обрезаем строку после '/'
            *last_slash = '\0';
            strcpy(new_path, panel->path);
        }
    }
    
    // Если выбрана директория, а не файл, то заходим в нее
    else if (newdir->d_type == DT_DIR) {
    
    // Заходим в поддиректорию из корня
        if (strcmp(panel->path, "/") == 0) {
            len = snprintf(new_path, sizeof(new_path), "/%s", newdir->d_name);
            if (len >= sizeof(new_path)) {
               fprintf(stderr, "Cлишком длинный путь. Ошибка snprintf\n");                
                return;
            }        
        // Заходим в поддиректорию из другой поддиректории    
        } else {
            len = snprintf(new_path, sizeof(new_path), "%s/%s", panel->path, newdir->d_name);
            if (len >= sizeof(new_path)) {
                fprintf(stderr, "Cлишком длинный путь. Ошибка snprintf\n");                
                return;
            }                
        }
    }
    else {
        return;
    }
    
    // Вызываем новую панель	
    load_panel(panel, new_path);
}

