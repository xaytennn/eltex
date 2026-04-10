#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 1024

typedef struct {
    char *left_args[MAX_ARGS];
    char *right_args[MAX_ARGS];
    char *input_file_left;
    char *output_file_left;
    char *input_file_right;
    char *output_file_right;
    int has_pipe;
    int isleft;
} Command;

void parse_command(char *input, Command *cmd);

#endif