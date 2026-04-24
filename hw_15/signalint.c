#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main (void){
    sigset_t set;
    int ret;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if(ret < 0){
        perror("sigaction set problem");
        exit(EXIT_FAILURE);
    }

    while(1){
        sleep(1);
    }
    exit(EXIT_SUCCESS);
}