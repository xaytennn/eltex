#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_ARGS 1024

int main(void){
    pid_t child_proc;
    int status, i;
    char *input = NULL;
    char *arguments[MAX_ARGS];
    char *token;
    char *input_file = NULL;
    char *output_file = NULL;
    
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
         
        // Находим название программы (первая лексема) и записываем введенную команду в массив
        input_file = NULL;
        output_file = NULL;
        i = 0;
        token = strtok(input, " \t");
        while (token != NULL && i < MAX_ARGS - 1) {
        
            // Проверяем, есть ли перенаправление ввода
            if(strcmp(token, "<") == 0){
                token = strtok(NULL, " \t");
                if(token != NULL){
                    input_file = token;
                }
            }
            
            // Проверяем, есть ли перенаправление вывода
            else if(strcmp(token, ">") == 0){
                token = strtok(NULL, " \t");
                if(token != NULL){
                    output_file = token;
                }
            }            
            
            else {
                arguments[i] = token;
                i++;
            }
            token = strtok(NULL, " \t");
        }
        arguments[i] = NULL;      
         
        child_proc = fork();
        if(child_proc == -1){
            perror("fork Child_pid error");
            free(input);
            exit(EXIT_FAILURE);
        }
        
        if(child_proc == 0){
            
            // Перенаправляем буфер
            if(input_file != NULL){
                int fd = open(input_file, O_RDONLY);
                if (fd == -1) {
                    perror("Ошибка при открытии файла для чтения");
                    free(input);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            
            if(output_file != NULL){
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1) {
                    perror("Ошибка при создании(открытии) файла для записи");
                    free(input);
                    exit(EXIT_FAILURE);
                }
    
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }            
            
            // Открываем программу через функцию execvp
            int execvp_check = execvp(arguments[0], arguments);            
            if(execvp_check == -1){
                perror("execvp error");
                free(input);
                exit(EXIT_FAILURE);
            }
            exit(2); 
        } else {
            // Ожидание завершения
            waitpid(child_proc, &status, 0);
        }
        free(input);
        input = NULL;            
    }
    exit(EXIT_SUCCESS);
}
