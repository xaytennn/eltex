#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define PORT 12345
#define COUNT_PTHREAD 3

// Создаем узел для односвязного списка
struct node{
    struct sockaddr_in client;
    struct node *next;
};

// Создаем начало и конец списка
struct node *head = NULL;
struct node *tail = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_server(void *arg){
    pthread_detach(pthread_self());
    int fd_server_thread_socket;
    int *id = (int *)arg;
    while(1){
        pthread_mutex_lock(&mutex);

        // Если конца нет, значит список пуст
        if (tail == NULL){
            pthread_mutex_unlock(&mutex);
            continue;
        }
        // Берем клиента из головы списка (по методу FIFO)
        struct node *node = head;
        head = head->next;
        // Если забрали последний элемент из головы, то удаляем его из конца
        if (head == NULL) tail = NULL;

        pthread_mutex_unlock(&mutex);

        // Берем эндпоинт клиента
        struct sockaddr_in client = node->client;
        free(node);
        fd_server_thread_socket = socket(AF_INET, SOCK_DGRAM, 0);
        
        if (fd_server_thread_socket < 0){
            perror("socket problem");
            free(arg);
            close(fd_server_thread_socket);
            pthread_exit(NULL);
        }

        if (connect(fd_server_thread_socket, (struct sockaddr*)&client, sizeof(client)) < 0) {
            perror("connect problem");
            free(arg);
            close(fd_server_thread_socket);
            pthread_exit(NULL);
        }

        time_t now = time(NULL);
        char *time = ctime(&now);
        if (send(fd_server_thread_socket, time, strlen(time), 0) < 0){
            perror("send problem");
            free(arg);
            close(fd_server_thread_socket);
            pthread_exit(NULL);
        }
        printf("Поток %lu под номером %d отправил : %s\n", pthread_self(), *id, time);

        close(fd_server_thread_socket);
    }
    free(arg);
    return NULL;
}

int main(void){
    int server_fd;
    struct sockaddr_in server, client;
    char buffer[1024] = {0};
    pthread_t thread[COUNT_PTHREAD];

    // Создаем сокет
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0){
        perror("socket problem");
        exit(EXIT_FAILURE);
    }    
    
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    // Привязываем сервер к порту и к любому ip компьютера
    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind problem");
        exit(EXIT_FAILURE);
    }

    printf("Сервер слушает порт %d\n", PORT);
    
    for (int i = 0; i < COUNT_PTHREAD; i++){
        int *id = malloc(sizeof(*id));
        *id = i;
        int ret = pthread_create(&thread[i], NULL, thread_server, id);
        if (ret != 0) {
            perror("pthread_create");
            free(id);
            close(server_fd);
        }
    }
    
    while(1){
        socklen_t client_len = sizeof(client);
        // Получаем сообщение и данные от клиента
        if (recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &client_len) < 0){
            perror("recv problem");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // Добавляем клиента в новый узел
        struct node *new_node = malloc(sizeof(*new_node));
        new_node->client = client;
        new_node->next = NULL;
        pthread_mutex_lock(&mutex);

        // Если список был пустым
        if (tail == NULL){
            tail = new_node;
            head = new_node;
        }

        // Если в списке уже были элементы
        else{
            tail->next = new_node;
            tail = new_node;
        }
        pthread_mutex_unlock(&mutex);
        printf("Сервер прочитал: %s\n", buffer);
    }

    for (int i = 0; i < COUNT_PTHREAD; i++){
        pthread_join(thread[i], NULL);
    }
    close(server_fd);

    return 0;
}