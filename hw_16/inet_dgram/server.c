#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void){
    int server_fd;
    struct sockaddr_in server, client;
    char buffer[1024] = {0};

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }    
    
    
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;
    
    // Привязываем сервер к порту и к любому ip компьютера
    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }

    printf("Сервер слушает порт 12345\n");

    
    socklen_t client_len = sizeof(client);

    // Получаем сообщение и данные от клиента
    if (recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &client_len)< 0){
        perror("recv problem");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Сервер прочитал: %s\n", buffer);

    // Отправляем сообщение обратно по сохраненным данным
    char *response = "Hi!\n";
    if (sendto(server_fd, response, strlen(response), 0, (struct sockaddr*)&client, client_len) < 0){
        perror("send problem");
        close(server_fd);
        exit(EXIT_FAILURE);        
    }
    
    close(server_fd);

    return 0;
}