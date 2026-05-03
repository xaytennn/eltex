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

    // Заполняем структуру для multicast-рассылки
    server.sin_family = AF_INET;
    server.sin_port = htons(7777);
    inet_pton(AF_INET, "224.0.0.1", &server.sin_addr);

    printf("Сервер раздает multicast на порт 7777\n");

    socklen_t server_len = sizeof(server);
    
    // Отправляем сообщение всем в группе
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