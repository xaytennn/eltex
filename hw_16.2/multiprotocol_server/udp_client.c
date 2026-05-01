#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PORT 12345

int main(void){
    int server_fd;
    struct sockaddr_in server, server_recv;
    char buffer[1024] = {0};    

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    socklen_t server_len = sizeof(server);
    socklen_t server_recv_len = sizeof(server_recv);

    // Отправляем сообщение серверу
    while (1){
        char *response = "What time is it now?\n";
        if(sendto(server_fd, response, strlen(response), 0, (struct sockaddr*) &server, server_len) < 0 ){
            perror("send problem");
            close(server_fd);
            exit(EXIT_FAILURE);        
        }

        // Получаем сообщение от сервера
        if (recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_recv, &server_recv_len) < 0){
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