#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "bash_core.h"


int bash_core(Command *cmd) { 
    pid_t child_proc;
    pid_t child_proc2 = -1;
    int status;   
    int pipefd[2];

    if(cmd->has_pipe == 1){
        if (pipe(pipefd) == -1) {
            perror("Pipe error");
            return -1;
        }
    }
    
    child_proc = fork();
    if(child_proc == -1){
        perror("fork Child_pid error");
        return -1;
    }
    
    if(child_proc == 0){
        cmd->isleft = 1;
        if(cmd->has_pipe == 1){
            close(pipefd[0]); // Закрываем дескриптор pipe на чтение
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }
        
        // Перенаправляем ввод из файла
        if(cmd->input_file_left){
            int fd = open(cmd->input_file_left, O_RDONLY);
            if (fd == -1) {
                perror("Ошибка при открытии файла для чтения");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        // Перенаправляем вывод в файл
        if(cmd->output_file_left){
                int fd = open(cmd->output_file_left, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1) {
                    perror("Ошибка при создании(открытии) файла для записи");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);            
        }

        // Запускаем полученную программу
        int execvp_check = execvp(cmd->left_args[0], cmd->left_args);
        if(execvp_check == -1){
            perror("execvp error");
            exit(EXIT_FAILURE);
        }  
        
        exit(2);
    }
    
    // Если есть пайп, то порождаем второй процесс
    if(cmd->has_pipe == 1){
        child_proc2 = fork();
        if(child_proc2 == -1){
            perror("fork Child_pid error");
            return -1;
        }
    
        if(child_proc2 == 0){
            close(pipefd[1]); // Закрываем дескриптор pipe на запись
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            // Перенаправляем ввод из файла в правой части от "|"
            if(cmd->input_file_right){
                    int fd = open(cmd->input_file_right, O_RDONLY);
                    if (fd == -1) {
                        perror("Ошибка при открытии файла для чтения");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
            }
        
            // Перенаправляем вывод в файл в правой части от "|"
            if(cmd->output_file_right){
                    int fd = open(cmd->output_file_right, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    if (fd == -1) {
                        perror("Ошибка при создании(открытии) файла для записи");
                        exit(EXIT_FAILURE);
                    }

                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }    
            // Запускаем полученную программу в правой части от "|"  
            int execvp_check = execvp(cmd->right_args[0], cmd->right_args); 
            if(execvp_check == -1){
                perror("execvp error");
                exit(EXIT_FAILURE);
            }

        }
        // Закрываем дескрипторы pipe после работы
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(child_proc2, &status, 0);                    
    }
    // Ожидание завершения
    waitpid(child_proc, &status, 0);
    return 0;
}
