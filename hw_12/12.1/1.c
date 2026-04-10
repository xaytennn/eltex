#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>


int main(void){
    pid_t child_pid1;
    int status;
    int fd[2];
    char hi_str[] = "Hi!";
    if (pipe(fd) == -1) {
        perror("Pipe error");
        exit(EXIT_FAILURE);
    }
    
    child_pid1 = fork();
    if (child_pid1 == -1) {
        perror("Fork Process1 error");
        exit(EXIT_FAILURE);
    }
    
    if(child_pid1 == 0){
        close(fd[1]); // Закрываем запись
        char buf[256];
        read(fd[0], buf, sizeof(buf));
        printf("Child process read: %s\n", buf);
        
        close(fd[0]); // Закрываем чтение, после того как использовали буфер
        exit(EXIT_SUCCESS);
    }
    else {
        close(fd[0]); // Закрываем чтение
        write(fd[1], hi_str, strlen(hi_str) + 1);
        close(fd[1]); // Закрываем запись после того, как использовали буфер
        
        waitpid(child_pid1, &status, 0);
        exit(EXIT_SUCCESS);
    }
}
