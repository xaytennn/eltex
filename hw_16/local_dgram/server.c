#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



int main(void){
    int server_fd;
    struct sockaddr_un server, client;
    char buffer[1024] = {0};
    
    // Создаем сокет для сервера
    server_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }    
    
    // Удаляем старый файловый сокет, если он ранее не удалился
    unlink("/tmp/dgram_serv");
    
    // Записываем путь до файлового сокета сервера
    server.sun_family = AF_LOCAL;
    strcpy(server.sun_path, "/tmp/dgram_serv");
    
    // Привязываемся к файловому сокету
    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }

    printf("Сервер слушает через /tmp/dgram_serv\n");

    
    socklen_t client_len = sizeof(client);

    // Получаем сообщение от клиента и записываем его данные
    if (recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &client_len)< 0){
        perror("recv problem");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Сервер прочитал: %s\n", buffer);

    // Отвечаем клиенту по его данным
    char *response = "Hi!\n";
    if (sendto(server_fd, response, strlen(response), 0, (struct sockaddr*)&client, client_len) < 0){
        perror("send problem");
        close(server_fd);
        exit(EXIT_FAILURE);        
    }
    
    // Закрываем сокет и удаляем файловый сокет
    close(server_fd);
    unlink("/tmp/dgram_serv");

    return 0;
}