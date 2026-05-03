#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>


int main(void){

    int listener_fd;
    char buffer[65535];

    // Создаем сокет
    listener_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (listener_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }

    // Получаем весь UDP-трафик
    while(1){
        ssize_t n = recv(listener_fd, buffer, sizeof(buffer), 0);
        if (n < 0){
            perror("recv problem");
            close(listener_fd);
            exit(EXIT_FAILURE);
        }
        // Смотрим, какие данные лежат. Для этого пропускаем заголовки IP и UDP
        int data_len = n - 28;  // 20 IP + 8 UDP
        printf("Raw-сокет прочитал (%d байт): %.*s\n", data_len, data_len, buffer + 28);

    }

    close(listener_fd);
    return 0;
}