#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void){
    int server_fd;
    struct sockaddr_in server;

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    // Разрешаем broadcast-рассылку на устройстве
    int flag = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

    // Настраиваем структуру для broadcast-рассылки
    server.sin_family = AF_INET;
    server.sin_port = htons(7777);
    inet_pton(AF_INET, "255.255.255.255", &server.sin_addr);

    printf("Сервер раздает broadcast на порт 7777\n");

    socklen_t server_len = sizeof(server);
    
    // Отправляем сообщение всем
    char *str = "Hello, i'm string from server!\n";
    for (int i = 0; i < 10; i++){
        if (sendto(server_fd, str, strlen(str), 0, (struct sockaddr*)&server, server_len) < 0){
            perror("send problem");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(server_fd);

    return 0;
}