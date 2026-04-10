#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>


int main(void){
    char hi_str[] = "Hi!";
    const char *fifo_path = "myfifo";
    if (mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo error");
        exit(EXIT_FAILURE);
    }
    
    int fd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        perror("open error");
        exit(EXIT_FAILURE);
    }
    
    write(fd, hi_str, strlen(hi_str) + 1);
    close(fd);

}
