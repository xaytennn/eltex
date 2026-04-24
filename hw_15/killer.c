#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void){
    int pid;
    printf("Введите pid программы, которой хотите отправить сигнал: \n");
    scanf("%d", &pid);
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill problem");
        exit(EXIT_FAILURE);
    }
    exit (EXIT_SUCCESS);
}