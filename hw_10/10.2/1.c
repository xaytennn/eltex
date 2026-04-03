#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(void){
    pid_t child_pid1, child_pid2;
    int status;
    
    child_pid1 = fork();
    if (child_pid1 == -1) {
        perror("Fork Process1 error");
        exit(EXIT_FAILURE);
    }
    
    // Порождение процесса 1
    if(child_pid1 == 0){
        
        // Порождение процесса 3
        pid_t child_pid3;
        
        child_pid3 = fork();
        if (child_pid3 == -1) {
            perror("Fork Process3 error");
            exit(EXIT_FAILURE);
        }
        
        if(child_pid3 == 0){
            printf("Process3 pid = %d\n", getpid());
	    printf("Process3 ppid = %d\n", getppid());
	    
	    // Завершение процесса 3
	    exit(3);
        }
        
        // Порождение процесса 4
        pid_t child_pid4;
        child_pid4 = fork();
        
        if (child_pid4 == -1) {
            perror("Fork Process4 error");
            exit(EXIT_FAILURE);
        }
        
        if(child_pid4 == 0){
            printf("Process4 pid = %d\n", getpid());
	    printf("Process4 ppid = %d\n", getppid());
	    
	    // Завершение процесса 4
	    exit(4);
        } else {
            printf("Process1 pid = %d\n", getpid());
            printf("Process1 ppid = %d\n", getppid());
            
            waitpid(child_pid3, &status, 0);
            printf("Process3 status = %d\n", WEXITSTATUS(status));
            
            waitpid(child_pid4, &status, 0);
            printf("Process4 status = %d\n", WEXITSTATUS(status));
            
            // Завершение процесса 1
            exit(1);
        }

    }
    
    child_pid2 = fork();
    
    if (child_pid2 == -1) {
        perror("Fork Process2 error");
        exit(EXIT_FAILURE);
    }
    if(child_pid2 == 0){
        
        // Порождение процесса 5
        pid_t child_pid5;
        
        child_pid5 = fork();
        if (child_pid5 == -1) {
            perror("Fork Process5 error");
            exit(EXIT_FAILURE);
        }        
        
        if(child_pid5 == 0){
            printf("Process5 pid = %d\n", getpid());
	    printf("Process5 ppid = %d\n", getppid());
	    
	    // Завершение процесса 5
	    exit(5);
        } else {
            printf("Process2 pid = %d\n", getpid());
            printf("Process2 ppid = %d\n", getppid());
            
            waitpid(child_pid5, &status, 0);
            printf("Process5 status = %d\n", WEXITSTATUS(status));
            
            // Завершение процесса 2
            exit(2);            
        
        }    
    }
    printf("Main pid = %d\n", getpid());
    printf("Main ppid = %d\n", getppid());
          
    waitpid(child_pid1, &status, 0);
    printf("Process1 status = %d\n", WEXITSTATUS(status));
    waitpid(child_pid2, &status, 0);
    printf("Process2 status = %d\n", WEXITSTATUS(status));
    
    exit(EXIT_SUCCESS);
}
