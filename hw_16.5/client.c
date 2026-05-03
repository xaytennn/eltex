#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct udp_header{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t check_sum;
};

int main(void){
    int server_fd;
    struct sockaddr_in server;
    char buffer[1500] = {0};  
    char read_buffer[1500] = {0};  
    struct udp_header my_udp;

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;
    socklen_t server_len = sizeof(server);

    // Заполняем заголовок udp
    my_udp.source_port = htons(33333);
    my_udp.destination_port = server.sin_port;
    char *response = "Hello!\n";
    int response_length = strlen(response);

    // Высчитываем общую длину udp-дейтаграммы, как сумму длины передаваеммых данных и размера стурктуры заголовка
    my_udp.length = htons(sizeof(my_udp) + response_length);

    // Чек-сумму не проверяем
    my_udp.check_sum = 0;

    // Сохраняем область памяти структуры my_udp в начало buffer
    memcpy(buffer, &my_udp, sizeof(my_udp));

    // Сохраняем область памяти передаваемых данных в buffer, после структуры my_udp
    memcpy(buffer + sizeof(my_udp), response, response_length);

    // Отправляем сообщение серверу
    if (sendto(server_fd, buffer, sizeof(my_udp) + response_length, 0, (struct sockaddr *)&server, server_len) < 0){
        perror("send problem");
        close(server_fd);
        exit(EXIT_FAILURE);        
    }

    printf("Клиент отправил: %s\n", response);
    
    while(1){
        // Получаем сообщение
        ssize_t n = recv(server_fd, read_buffer, sizeof(read_buffer), 0);
        if (n < 0){
            perror("recv problem");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Находим в принятом потоке данных порт получателя и проверяем, чтобы он был наш
        uint16_t check_for_my_port = *(uint16_t *)(read_buffer+22); // 20 IP + 2 до UDP.destination_port
        if(my_udp.source_port == check_for_my_port){
            int data_len = n - 28;  // 20 IP + 8 UDP
            printf("Raw-сокет прочитал (%d байт): %.*s\n", data_len, data_len, read_buffer + 28);
        }
    }


    close(server_fd);
    return 0;
}