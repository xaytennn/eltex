#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>


int main(void){
    char read_str[256];
    const char *fifo_path = "myfifo";
    
    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1) {
        perror("open error");
        exit(EXIT_FAILURE);
    }
    
    ssize_t bytes = read(fd, read_str, strlen(read_str) - 1);
        
    if (bytes > 0) {
        read_str[bytes] = '\0';
        printf("Client read: %s\n", read_str);
    }
    
    close(fd);
    
    if (unlink(fifo_path) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }    
}
