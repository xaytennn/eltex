#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void){
    pid_t child_pid;
    int status;
    
    child_pid = fork();
    
    if(child_pid == -1){
        perror("fork Child_pid error");
        exit(EXIT_FAILURE);
    }    
    
    if(child_pid == 0){
        printf("Child pid = %d\n", getpid());
        printf("Child ppid = %d\n", getppid());
        exit(5);
    } else {
        printf("Parent pid = %d\n", getpid());
        printf("Parent ppid = %d\n", getppid());
        waitpid(child_pid, &status, 0);
        printf("status = %d\n", WEXITSTATUS(status));
    }
    
    exit(EXIT_SUCCESS);

}
