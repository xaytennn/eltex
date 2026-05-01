#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/epoll.h>

#define PORT 12345
#define MAX_EVENTS 50

int main(void){
    int server_udp_fd, server_tcp_fd, client_tcp_fd;
    struct sockaddr_in server, client;
    char buffer[1024] = {0};
    int epoll_base = epoll_create1 (0);
    struct epoll_event epoll_fd, events[MAX_EVENTS];
    int ev_count = 0;

    // Создаем сокет
    server_udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_udp_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }
    
    server_tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_tcp_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    // Привязываем сервер к порту и к любому ip компьютера
    if (bind(server_udp_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }
    // Создаем событие для UDP сокета
    epoll_fd.data.fd = server_udp_fd;
    epoll_fd.events = EPOLLIN;
    epoll_ctl(epoll_base, EPOLL_CTL_ADD, server_udp_fd, &epoll_fd);

    if (bind(server_tcp_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }
    listen(server_tcp_fd, 5);
    printf("Сервер слушает порт %d\n", PORT);

    // Создаем событие для TCP сокета
    epoll_fd.data.fd = server_tcp_fd;
    epoll_fd.events = EPOLLIN;
    epoll_ctl(epoll_base, EPOLL_CTL_ADD, server_tcp_fd, &epoll_fd);


    while(1){

        // Ожидаем события
        ev_count = epoll_wait(epoll_base, events, MAX_EVENTS, -1);

        // В массиве событий смотрим, что пришло
        for (int i = 0; i < ev_count; i++){
            
            // Если пришло событие на UDP сокет
            if(events[i].data.fd == server_udp_fd){
                socklen_t client_len = sizeof(client);

                // Получаем сообщение и данные от клиента
                if (recvfrom(server_udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &client_len)< 0){
                    perror("recv problem");
                    close(server_udp_fd);
                    exit(EXIT_FAILURE);
                }
                printf("Сервер через UDP прочитал: %s\n", buffer);

                time_t now = time(NULL);
                char *time = ctime(&now);
                if (sendto(server_udp_fd, time, strlen(time), 0, (struct sockaddr*)&client, client_len) < 0){
                    perror("send problem");
                    close(server_udp_fd);
                    exit(EXIT_FAILURE);
                }
                printf("UDP отправил : %s\n", time);
            }

            // Если пришло событие на TCP сокет
            else if(events[i].data.fd == server_tcp_fd){
                socklen_t client_len = sizeof(client);
                client_tcp_fd = accept(server_tcp_fd, (struct sockaddr*)&client, &client_len);
                if (client_tcp_fd < 0) {
                    perror("accept problem");
                    exit(EXIT_FAILURE);
                }

                // Сохраняем событие на подключенного клиента TCP для повторного чтения сообщений
                epoll_fd.data.fd = client_tcp_fd;
                epoll_fd.events = EPOLLIN;
                epoll_ctl(epoll_base, EPOLL_CTL_ADD, client_tcp_fd, &epoll_fd);
                
            }
            else{

                // Работаем с событиями от клиента
                int fd = events[i].data.fd;
                if (recv(fd, buffer, sizeof(buffer), 0)< 0){
                    perror("recv problem");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                printf("Сервер через TCP прочитал: %s\n", buffer);
    
                time_t now = time(NULL);
                char *time = ctime(&now);
                if (send(fd, time, strlen(time), 0) < 0){
                    perror("send problem");
                    close(fd);
                    exit(EXIT_FAILURE);        
                }
                printf("TCP отправил : %s\n", time);

            }
        }
    }

    // Закрываем сокеты
    close(server_udp_fd);
    close(server_tcp_fd);
    close(client_tcp_fd);

    return 0;
}