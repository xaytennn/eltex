#include "server.h"

Client clients[10];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Рассылка всем клиентам
void broadcast(struct chat_msg *msg){
        for (int i = 0; i < client_count; i++) {
            if (clients[i].active) {
                mq_send(clients[i].mq, (char*)msg, sizeof(chat_msg), 0);
            }
        }
}

// Сохраняем историю сообщений
void add_chat_history(struct chat_msg *msg){
    // Храним в перезаписывающемся массиве (кольцевой буфер)
        history[hist_index] = *msg;
        hist_index = (hist_index + 1) % MAX_HISTORY;
        if (hist_count < MAX_HISTORY) hist_count++;
}

// Отправляем историю клиенту
void send_history_to_client(mqd_t client_mq) {
    int start = hist_index - hist_count;
    if (start < 0) start += MAX_HISTORY;

    for (int i = 0; i < hist_count; i++) {
        int idx = (start + i) % MAX_HISTORY;
        mq_send(client_mq, (char*)&history[idx], sizeof(struct chat_msg), 0);
    }
}

// Обновляем список активных клиентов
void send_users_list_to_all() {
    struct chat_msg msg;
    msg.type = USERS_LIST;
    strcpy(msg.sender, "SERVER");

    char list[MAX_MSG] = "";

    for (int i = 0; i < client_count; i++) {
        if (clients[i].active) {
            strcat(list, clients[i].name);
            strcat(list, "\n");
        }
    }

    strcpy(msg.text, list);
    broadcast(&msg);
}

int main (void){
    mqd_t chat_mq;
    struct chat_msg msg, sys_msg;
    struct mq_attr attr = {0, 10, sizeof(struct chat_msg), 0};
    mq_unlink("/chat_queue");

    // Удаляем очереди клиентов
    for (int i = 0; i < MAX_CLIENTS; i++) {
        char qname[64];
        snprintf(qname, 64, "/client_%d", i);
        mq_unlink(qname);
    }

    chat_mq = mq_open("/chat_queue", O_CREAT  | O_RDWR, 0666, &attr);
    if(chat_mq == -1){
        perror("mq_open problem");
        exit(EXIT_FAILURE);
    }

    while(1){

        // Получаем сообщение от клиента
        ssize_t len = mq_receive(chat_mq, (char*)&msg, sizeof(msg), NULL);

        if (len == -1) {
            perror("mq_receive problem");
            exit(EXIT_FAILURE);
        }

        switch(msg.type){

            // Клиент подключился
            case(JOIN_CLIENT):

                pthread_mutex_lock(&clients_mutex);

            // Создаем отдельную очередь для каждого клиента
                    snprintf(clients[client_count].qname, 64, "/client_%d", client_count);
                    clients[client_count].mq = mq_open(clients[client_count].qname, O_CREAT | O_WRONLY, 0666, &attr);
                    strcpy(clients[client_count].name, msg.sender);
                    snprintf(sys_msg.text, MAX_MSG, "%s joined the chat", msg.sender);
                    clients[client_count].active = 1;
                    sys_msg.type = TEXT_CLIENT;
                    strcpy(sys_msg.sender, "SERVER");

                    // Отправляем имя очереди
                    struct chat_msg response;
                    response.type = JOIN_CLIENT;
                    strcpy(response.sender, "SERVER");
                    strcpy(response.text, clients[client_count].qname);
                    mq_send(chat_mq, (char*)&response, sizeof(response), 0);

                    // Сохраняем и выводим сообщение о подключении
                    add_chat_history(&sys_msg);

                    // Отправляем историю клиенту
                    send_history_to_client(clients[client_count].mq);

                    // Рассылка всем
                    broadcast(&sys_msg);
                    client_count++;
                    send_users_list_to_all();

                pthread_mutex_unlock(&clients_mutex);
                break;
            // Клиент отключился
            case(LEFT_CLIENT):
                for(int i = 0; i < MAX_CLIENTS; i++){
                    if(strcmp(clients[i].name, msg.sender) == 0){

                        // Очищаем данные и очередь вышедшего клиента
                        mq_close(clients[i].mq);
                        mq_unlink(clients[i].qname);
                        clients[i].active = 0;
                        for (int j = i; j < client_count - 1; j++) {
                            clients[j] = clients[j + 1];
                        }

                        client_count--;
                    }
                }

                // Выводим и удаляем сообщение о выходе
                snprintf(sys_msg.text, MAX_MSG, "%s left the chat", msg.sender);
                sys_msg.type = TEXT_CLIENT;
                strcpy(sys_msg.sender, "SERVER");
                pthread_mutex_lock(&clients_mutex);
                add_chat_history(&sys_msg);
                broadcast(&sys_msg);
                send_users_list_to_all();
                pthread_mutex_unlock(&clients_mutex);
                break;

            // Клиент пишет сообщение
            case(TEXT_CLIENT):

                // Выводим и сохраняем написанное клиентом сообщение
                pthread_mutex_lock(&clients_mutex);
                add_chat_history(&msg);
                broadcast(&msg);
                pthread_mutex_unlock(&clients_mutex);
                break;
        }
}

    // Очищаем ресурсы
    mq_close(chat_mq);
    mq_unlink("/chat_queue");
    return 0;
}