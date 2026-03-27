#include "my_mc.h"

int current_selection = 0;


int main(void) {
    int ch;
    int tab = 0;
    int height, width;
    Panel right = {0}, left = {0};
    
    initscr();                 // Инициализация ncurses
    noecho();                  // Не выводить нажатые клавиши
    curs_set(0);               // Скрыть курсор
    keypad(stdscr, TRUE);
    clear();
    start_color();   
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    bkgd(COLOR_PAIR(1));  
    refresh();
    getmaxyx(stdscr, height, width);
    mvprintw(height-2, 3, "Navigation: arrows keys, Tab - switch, q - exit");
    
    // Задаем расположение и фон для двух окон
    left.wnd = newwin(height-4, width/2-2, 2, 2);
    wbkgd(left.wnd, COLOR_PAIR(1));
       
    right.wnd = newwin(height-4, width/2, 2, width/2);
    wbkgd(right.wnd, COLOR_PAIR(1));
    
    // Загружаем стартовые данные из директорий в панель
    load_panel(&left, "/");
    load_panel(&right, "/home");
    
    wrefresh(left.wnd);    
    wrefresh(right.wnd);

    while(1){
    
        // Рисуем список каталогов и файлов в два окна
        draw_namelist(&left, tab == 0);
        draw_namelist(&right, tab == 1);

        refresh();
        ch = getch();
        wclear(left.wnd);
        wclear(right.wnd);    
    
        switch (ch) {  
            case KEY_UP:
                if (tab == 0 && left.current_selection > 0) left.current_selection--;
                else if (tab == 1 && right.current_selection > 0) right.current_selection--;
                break;
                
            case KEY_DOWN:
                if (tab == 0 && left.current_selection < left.n - 1) left.current_selection++;
                else if (tab == 1 && right.current_selection < right.n - 1) right.current_selection++;
                break;
            
            case '\t':
                tab = !tab;
                break;
            
            case '\n':
                if (tab == 0) enter_directory(&left);
                else enter_directory(&right);
                break;          

            case 'q':
                delwin(left.wnd);    
                delwin(right.wnd);
                	    
                if (left.namelist) {
                    for (int i = 0; i < left.n; i++) {
                        free(left.namelist[i]);
                    }
                    free(left.namelist);
		    left.namelist = NULL;
		}
	    
	        if (right.namelist) {
                    for (int i = 0; i < right.n; i++) {
                        free(right.namelist[i]);
                    }
                    free(right.namelist);
                    right.namelist = NULL;
                }
                endwin();
                return 0;
        
            default:
                break;
        }    
    }    

    endwin();
    return 0;
}

