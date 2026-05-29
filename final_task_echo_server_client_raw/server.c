#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include "connection_defs.h"

struct client *head = NULL;
int running = 1;

// Ищем предыдущего клиента в односвязном списке
struct client* find_prev_client_in_linked_list(struct client *client){
    if (client == NULL) return NULL;
    struct client *prev = NULL;
    struct client *current = head;
    while(current != NULL && current != client){
        prev = current;
        current = current->next;
    }
    if (current == NULL) return NULL;

    return prev;
}

// Удаляем найденного клиента из односвязного списка
void delete_client_in_linked_list(struct client *client){
    if(client == NULL) return;
    struct client *prev = find_prev_client_in_linked_list(client);
    if(prev == NULL){
        head = head->next;
    }
    else{
        prev->next = client->next;
    }
    free(client);
}

// Добавляем клиента в начало односвязного списка
void add_client_in_linked_list(struct client *new_client){
    struct client *node = malloc(sizeof(*node));
    *node = *new_client;
    node->next = head;
    head = node;
}

// Ищем есть ли у нас клиент с нужным портом и ip-адресом
struct client* check_exist_client_by(uint16_t port, uint32_t ip){

    for(struct client *c = head; c; c = c->next){
        if(*(uint32_t *)(c->packet_ip_udp_data + SOURCE_IP_OFFSET) == ip && *(uint16_t *)(c->packet_ip_udp_data + SOURCE_PORT_OFFSET) == port){
            return c;
        }
    }
    return NULL;
}

// Очищаем память, выделенную на клиентов
void clean_clients_memory(){
    struct client *c = head;
    while (c) {
        struct client *next = c->next;
        delete_client_in_linked_list(c);
        c = next;
    }
    head = NULL;
}

// Отправляем сообщение определенному клиенту
void send_response_to_client(int fd_server_thread_socket, struct client *new_client){
    char write_buffer[MAX_DATA_SIZE] = {0};  
    struct udp_header my_udp = {0};
    struct iphdr ip = {0};
    ip = init_iphdr(*(uint32_t*)(new_client->packet_ip_udp_data + DESTINATION_IP_OFFSET));

    // Заполняем заголовок UDP
    my_udp.source_port = htons(SERVER_PORT);
    my_udp.destination_port = *(uint16_t *)(new_client->packet_ip_udp_data + SOURCE_PORT_OFFSET);

    while(1){
        
        // Отключаемся от клиента, если он написал exit
        if (strcmp("exit", new_client->packet_ip_udp_data + PAYLOAD_OFFSET) == 0){
            printf("Завершаю поток для клиента [IP %s:PORT %d]\n", inet_ntoa(new_client->client_endpoint.sin_addr), ntohs(my_udp.destination_port));
            delete_client_in_linked_list(new_client);
            return;
        }

        char response[MAX_DATA_SIZE] = {0};
        if(running){
            snprintf(response, sizeof(response), "%s %d", new_client->packet_ip_udp_data + PAYLOAD_OFFSET, new_client->message_count);
        }
        else{
            strcpy(response, "connection denied");
        }
        int response_length = strlen(response);
        new_client->message_count++;

        // Высчитываем общую длину UDP-дейтаграммы, как сумму длины передаваеммых данных и размера стурктуры заголовка
        my_udp.length = htons(sizeof(my_udp) + response_length);

        memset(write_buffer, 0, sizeof(write_buffer));
        memcpy(write_buffer, &ip, sizeof(ip));
        memcpy(write_buffer + sizeof(ip), &my_udp, sizeof(my_udp));
        memcpy(write_buffer + sizeof(ip) + sizeof(my_udp), response, response_length);

        // Отправляем сообщение серверу
        socklen_t client_len = sizeof(new_client->client_endpoint);
        ssize_t n = sendto(fd_server_thread_socket, write_buffer, sizeof(ip) + sizeof(my_udp) + response_length, 0, (struct sockaddr *)&new_client->client_endpoint, client_len);
        if(n < 0){
            perror("send problem");
            clean_clients_memory();
            close(fd_server_thread_socket);
            exit(EXIT_FAILURE);        
        }
        int data_len = n - PAYLOAD_OFFSET;
        printf("[IP %s:PORT %d] ", inet_ntoa(new_client->client_endpoint.sin_addr), ntohs(my_udp.destination_port));
        printf("Raw-сокет отправил (%d байт): %.*s\n", data_len, data_len, write_buffer + PAYLOAD_OFFSET);
        break;
    }
}


// Выключаем сервер, если поймали сигнал
void close_server(int sig) {
    running = 0; 
}

int main(void){

    // Перехватываем сигналы завершения, чтобы сообщить клиентам, что сервер отключился
    struct sigaction handler = {0};
    handler.sa_handler = close_server;
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);
    sigaction(SIGTERM, &handler, NULL);

    int server_fd;
    struct sockaddr_in server;
    struct client new_client;
    char read_buffer[MAX_DATA_SIZE] = {0};
    server_fd = init_raw_socket();
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);

    while(running){
        socklen_t client_len = sizeof(new_client.client_endpoint);

        // Получаем сообщение
        memset(read_buffer, 0, sizeof(read_buffer));
        ssize_t n = recvfrom(server_fd, read_buffer, sizeof(read_buffer), 0, (struct sockaddr*)&new_client.client_endpoint, &client_len);
        if (n < 0){
            if(errno == EINTR && !running){
                break;
            }
            perror("recv problem");
            clean_clients_memory();
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Находим в принятом потоке данных порт получателя и проверяем, чтобы он был наш
        uint16_t check_for_my_port = *(uint16_t *)(read_buffer+DESTINATION_PORT_OFFSET); // 20 IP + 2 до UDP.destination_port
        if(server.sin_port == check_for_my_port){
            int data_len = n - PAYLOAD_OFFSET;
            printf("Сервер Raw-сокет прочитал (%d байт): %.*s\n", data_len, data_len, read_buffer + PAYLOAD_OFFSET);
            memset(new_client.packet_ip_udp_data, 0, sizeof(new_client.packet_ip_udp_data));
            memcpy(new_client.packet_ip_udp_data, read_buffer, n);
            
            uint16_t client_port = *(uint16_t *)(read_buffer+SOURCE_PORT_OFFSET);
            uint32_t client_ip = *(uint32_t *)(read_buffer+SOURCE_IP_OFFSET);
            struct client *exist_client = check_exist_client_by(client_port, client_ip);
            
            // Создаем клиента
            if(exist_client == NULL){
                new_client.message_count = 1;
                add_client_in_linked_list(&new_client);
                send_response_to_client(server_fd, head);
            }
            else{
                // Работаем с существующим клиентом
                memset(exist_client->packet_ip_udp_data, 0, sizeof(exist_client->packet_ip_udp_data));
                memcpy(exist_client->packet_ip_udp_data, new_client.packet_ip_udp_data, n);
                send_response_to_client(server_fd, exist_client);
            }
        }
    }

    // Отправляем всем уведомление, что сервер отключился
    printf("\nGot a signal. Server and clients will close\n");
    for(struct client *c = head; c; c = c->next){
        send_response_to_client(server_fd, c);
    }

    // Очищаем память, выделенную на клиентов
    clean_clients_memory();

    close(server_fd);
    exit(EXIT_SUCCESS);
}