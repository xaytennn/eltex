#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main (void){
    sigset_t set;
    int ret;
    int sig_num;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if(ret < 0){
        perror("sigaction set problem");
        exit(EXIT_FAILURE);
    }

    while(1){
        sigwait(&set, &sig_num);
        printf("Sig number %d\n", sig_num);
    }
    exit(EXIT_SUCCESS);
}