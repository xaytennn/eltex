#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parser.h"
#include "bash_core.h"

#define MAX_ARGS 1024

int main(void){
    char *input = NULL;

    Command cmd;
    
    while(1){
        // Считываем имя программы и опции запуска из командной строки
        char *prompt = "\033[94mВведите имя программы и опции запуска:\n\033[0m\033[1;32mmy_bash> \033[0m";
        input = readline(prompt);
        if (input == NULL) {
            printf("Ошибка ввода\n");
            exit(EXIT_FAILURE);
        }
        
        // Выход итерации, если пустая строка
        if (strlen(input) == 0) {
            free(input);
            continue;
        }
        
        // Сохранение истории запросов в readline
        add_history(input);
        
        if (strcmp(input, "exit") == 0) {
            free(input);
            break;
        }
        
        // Вызов функции для парсинга строки
        parse_command(input, &cmd);

        // Вызов функции реализующей работу my_bash
        int err_check = bash_core(&cmd);
        if (err_check != 0){
            free(input);
            exit(EXIT_FAILURE);
        }

        free(input);
        input = NULL;        
    }
    exit(EXIT_SUCCESS);
}
