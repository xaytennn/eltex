#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(void){
    int client_fd;
    struct sockaddr_un server, client;
    char buffer[1024] = {0};    

    client_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (client_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }
    
    // Удаляем старый файловый сокет, если он ранее не удалился
    unlink("/tmp/dgram_client");
    client.sun_family = AF_LOCAL;
    
    // Задаем файловый сокет для клиента
    strcpy(client.sun_path, "/tmp/dgram_client");

    // Привязываем клиента к файловому сокету
    if (bind(client_fd, (struct sockaddr*)&client, sizeof(client)) < 0) {
        perror("client bind problem");
        exit(EXIT_FAILURE);
    }
    
    // Задаем файловый сокет для сервера
    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "/tmp/dgram_serv");

    socklen_t server_len = sizeof(server);

    // Подключаемся к серверу
    if (connect(client_fd, (struct sockaddr*)&server, server_len) < 0) {
        perror("connect problem");
        exit(EXIT_FAILURE);
    }

    // Отправляем сообщение серверу
    char *response = "Hello!\n";
    if (send(client_fd, response, strlen(response), 0) < 0){
        perror("send problem");
        close(client_fd);
        exit(EXIT_FAILURE);        
    }

    // Получаем сообщение от сервера
    if (recv(client_fd, buffer, sizeof(buffer), 0) < 0){
        perror("recv problem");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Клиент прочитал: %s\n", buffer);
    
    // Закрываем сокет и удаляем файловый сокет
    close(client_fd);
    unlink("/tmp/dgram_client");
    return 0;
}