#include <unistd.h>
#include <string.h>
#include "connection_defs.h"

int main(void){
    int server_fd;
    struct sockaddr_in server;
    char buffer[MAX_DATA_SIZE] = {0};  
    char read_buffer[MAX_DATA_SIZE] = {0};  
    struct udp_header my_udp = {0};
    struct iphdr ip = {0};

    // Создаем сокет
    server_fd = init_raw_socket();

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);
    socklen_t server_len = sizeof(server);
    
    // Заполняем IP-заголовок
    ip = init_iphdr(server.sin_addr.s_addr);
    memcpy(buffer, &ip, sizeof(ip));

    // Заполняем заголовок UDP
    my_udp.source_port = htons(30000 + (getpid() % 30000)); // Генерируем port через pid, чтобы он не повторялся от клиента к клиенту
    my_udp.destination_port = server.sin_port;
    printf("Enter your message. To close the connection, type exit (without quotes)\n");
    while(1){
        memset(buffer + PAYLOAD_OFFSET, 0, sizeof(buffer) - PAYLOAD_OFFSET);
        
        // Создаем строку с сообщением
        char response[MAX_DATA_SIZE] = {0};
        scanf("%1499s", response);
        int response_length = strlen(response);
    
        // Высчитываем общую длину UDP-дейтаграммы, как сумму длины передаваеммых данных и размера стурктуры заголовка
        my_udp.length = htons(sizeof(my_udp) + response_length);
        // Сохраняем область памяти структуры my_udp в buffer, после IP заголовка
        memcpy(buffer + sizeof(ip), &my_udp, sizeof(my_udp));
        // Сохраняем область памяти передаваемых данных в buffer, после структуры my_udp
        memcpy(buffer + sizeof(ip) + sizeof(my_udp), response, response_length);


        // Отправляем сообщение серверу
        if (sendto(server_fd, buffer, sizeof(ip) + sizeof(my_udp) + response_length, 0, (struct sockaddr *)&server, server_len) < 0){
            perror("send problem");
            close(server_fd);
            exit(EXIT_FAILURE);        
        }

        printf("Клиент отправил: %s\n", response);
        if(strcmp("exit", response) == 0){
            close(server_fd);
            exit(EXIT_SUCCESS);
        }

        while(1){
            // Получаем сообщение
            memset(read_buffer, 0, sizeof(read_buffer));
            ssize_t n = recv(server_fd, read_buffer, sizeof(read_buffer), 0);
            if (n < 0){
                perror("recv problem");
                close(server_fd);
                exit(EXIT_FAILURE);
            }

            // Находим в принятом потоке данных порт получателя и проверяем, чтобы он был наш
            uint16_t check_for_my_port = *(uint16_t *)(read_buffer+DESTINATION_PORT_OFFSET); // 20 IP + 2 до UDP.destination_port
            if(my_udp.source_port == check_for_my_port){
                int data_len = n - PAYLOAD_OFFSET;  // 20 IP + 8 UDP
                printf("Raw-сокет прочитал (%d байт): %.*s\n", data_len, data_len, read_buffer + PAYLOAD_OFFSET);
                if(strcmp(read_buffer + PAYLOAD_OFFSET, "connection denied") == 0){
                    close(server_fd);
                    exit(EXIT_SUCCESS);
                }
                break;
            }
        }
    }

    close(server_fd);
    exit(EXIT_SUCCESS);
}