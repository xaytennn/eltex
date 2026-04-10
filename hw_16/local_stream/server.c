#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(void){
    int server_fd, client_fd;
    struct sockaddr_un server, client;
    char buffer[1024] = {0};
    server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "/tmp/stream_serv");

    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 5);
    printf("Сервер слушает через /tmp/stream_serv\n");

    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr*)&client, &client_len);

    if (client_fd < 0) {
        perror("accept problem");
        exit(EXIT_FAILURE);
    }
    
    if (recv(client_fd, buffer, sizeof(buffer), 0)< 0){
        perror("recv problem");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Сервер прочитал: %s\n", buffer);

    char *response = "Hi!\n";
    if (send(client_fd, response, strlen(response), 0) < 0){
        perror("send problem");
        close(client_fd);
        exit(EXIT_FAILURE);        
    }

    close(client_fd);
    close(server_fd);
    unlink("/tmp/stream_serv");

    return 0;
}