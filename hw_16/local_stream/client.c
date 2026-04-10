#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(void){
    int server_fd;
    struct sockaddr_un server;
    char buffer[1024] = {0};    

    server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "/tmp/stream_serv");

    socklen_t server_len = sizeof(server);
    if (connect(server_fd, (struct sockaddr*)&server, server_len) < 0) {
        perror("connect problem");
        exit(EXIT_FAILURE);
    }

    char *response = "Hello!\n";
    if (send(server_fd, response, strlen(response), 0) < 0){
        perror("send problem");
        close(server_fd);
        exit(EXIT_FAILURE);        
    }    
    
    if (recv(server_fd, buffer, sizeof(buffer), 0)< 0){
        perror("recv problem");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Клиент прочитал: %s\n", buffer);
    
    close(server_fd);
    return 0;
}