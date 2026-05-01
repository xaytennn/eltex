#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(void){
    int server_fd;
    struct sockaddr_in server;
    char buffer[1024] = {0};    

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;

    // Подключаемся к серверу
    socklen_t server_len = sizeof(server);
    if (connect(server_fd, (struct sockaddr*)&server, server_len) < 0) {
        perror("connect problem");
        exit(EXIT_FAILURE);
    }
    while (1){
        
        // Шлем сообщение серверу
        char *response = "What time is it now?\n";
        if (send(server_fd, response, strlen(response), 0) < 0){
            perror("send problem");
            close(server_fd);
            exit(EXIT_FAILURE);        
        }    
        
        // Получаем сообщние от сервера
        if (recv(server_fd, buffer, sizeof(buffer), 0)< 0){
            perror("recv problem");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Клиент прочитал: %s\n", buffer);
        usleep(1000000);
    }
    close(server_fd);
    return 0;
}