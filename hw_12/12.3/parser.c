#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void parse_command(char *input, Command *cmd){
    // Находим название программы (первая лексема) и записываем введенную команду в массив
    int i = 0;
    char *token;
    cmd->input_file_left = NULL;
    cmd->output_file_left = NULL;
    cmd->input_file_right = NULL;
    cmd->output_file_right = NULL;
    cmd->has_pipe = 0;
    cmd->isleft = 1;
    token = strtok(input, " \t");
    while (token != NULL && i < MAX_ARGS - 1){

        // Проверяем, есть ли пайп
        if (strcmp(token, "|") == 0){
            cmd->has_pipe = 1;
            cmd->left_args[i] = NULL;
            i = 0;
            cmd->isleft = 0;
        }

        // Проверяем, есть ли перенаправление ввода
        else if (strcmp(token, "<") == 0){
            token = strtok(NULL, " \t");
            if (token != NULL){
                if(cmd->isleft){
                    cmd->input_file_left = token;
                }
                else{
                    cmd->input_file_right = token;
                }
            }
        }

        // Проверяем, есть ли перенаправление вывода
        else if (strcmp(token, ">") == 0){
            token = strtok(NULL, " \t");
            if (token != NULL){
                if(cmd->isleft){
                    cmd->output_file_left = token;
                }
                else{
                    cmd->output_file_right = token;
                }
            }
        }

        // Записываем аргументы, которые будет использовать для запуска программы
        else if (cmd->has_pipe == 0){
            cmd->left_args[i] = token;
            i++;
        }
        else if (cmd->has_pipe == 1){
            cmd->right_args[i] = token;
            i++;
        }
        token = strtok(NULL, " \t");
    }
    
    if (cmd->has_pipe == 1){
        cmd->right_args[i] = NULL;
    }
    else{
        cmd->left_args[i] = NULL;
    }
}