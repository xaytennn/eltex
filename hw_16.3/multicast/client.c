#define _GNU_SOURCE
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
    char buffer[1024] = {0};    

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }
    struct ip_mreqn multi_group;
    
    // Создаем мультигруппу и заполняем значения структуры
    multi_group.imr_address.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "224.0.0.1", &multi_group.imr_multiaddr);
    multi_group.imr_ifindex = 0;
    setsockopt(server_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multi_group, sizeof(multi_group));

    server.sin_family = AF_INET;
    server.sin_port = htons(7777);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t server_len = sizeof(server);

    if (bind(server_fd, (struct sockaddr*)&server, server_len) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++){
        // Получаем сообщение от сервера
        if (recv(server_fd, buffer, sizeof(buffer), 0) < 0){
            perror("recv problem");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
    
        printf("Клиент прочитал: %s\n", buffer);
    }
    
    close(server_fd);
    return 0;
}