#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(void){
    int server_fd, client_fd;
    struct sockaddr_in server, client;
    char buffer[1024] = {0};
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 5);
    printf("Сервер слушает порт 12345\n");

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

    return 0;
}