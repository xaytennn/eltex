#define _GNU_SOURCE
#include "chat_defs.h"


// Сохраняем историю сообщений
void add_chat_history(chat_t *chat, chat_msg_t *msg) {
    chat->history[chat->hist_index] = *msg;
    chat->hist_index = (chat->hist_index + 1) % MAX_HISTORY;
    if (chat->hist_count < MAX_HISTORY) chat->hist_count++;
}

// Очищаем ресурсы, если поймали сигнал
void cleanup_server(int sig) {
    printf("Got a signal. Safe exit\n");
    shm_unlink(SHM_PATH);
    sem_unlink(SEM_SERVER_PATH);
    sem_unlink(SEM_CLIENT_PATH);
    sem_unlink(SEM_MUTEX_PATH);
    sem_unlink(SEM_NEW_MSG_PATH);
    exit(EXIT_SUCCESS);
}

int main(void){
    // Ловим сигналы, чтобы очистить ресурсы
    struct sigaction handler;
    handler.sa_handler = cleanup_server;
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);
    sigaction(SIGTERM, &handler, NULL);

    // Если вдруг не очистилось при выходе из программы
    shm_unlink(SHM_PATH);
    sem_unlink(SEM_SERVER_PATH);
    sem_unlink(SEM_CLIENT_PATH);
    sem_unlink(SEM_MUTEX_PATH);
    sem_unlink(SEM_NEW_MSG_PATH);

    // Создаем семафоры
    sem_t *server_semaphore = sem_open(SEM_SERVER_PATH, O_CREAT | O_EXCL, 0666, 1);
    if(server_semaphore == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    sem_t *client_semaphore = sem_open(SEM_CLIENT_PATH, O_CREAT | O_EXCL, 0666, 0);
    if(client_semaphore == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    sem_t *mutex = sem_open(SEM_MUTEX_PATH, O_CREAT | O_EXCL, 0666, 1);
    if(mutex == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    sem_t *new_msg_sem = sem_open(SEM_NEW_MSG_PATH, O_CREAT | O_EXCL, 0666, 0);
    if(new_msg_sem == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }

    // Создаем разделяемую память
    int fd = shm_open(SHM_PATH, O_CREAT | O_RDWR | O_EXCL, 0666);
    if(fd == -1){
        perror("shm_open problem");
        exit(EXIT_FAILURE);
    }

    // Меняем размер разделенной памяти
    if(ftruncate(fd, SHM_SIZE) == -1){
        perror("ftruncate problem");
        exit(EXIT_FAILURE);
    }

    // Отображаем память в виртуальном адресном пространстве
    chat_t *chat = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(chat == MAP_FAILED){
        perror("mmap problem");
        exit(EXIT_FAILURE);
    }

    memset(chat, 0, SHM_SIZE);
    while(1){
        sem_wait(server_semaphore);
        sem_wait(mutex);
        chat_msg_t msg_copy = chat->new_msg;
        sem_post(mutex);
        chat_msg_t sys_msg;
        switch(msg_copy.type){

            // Клиент подключается
            case(JOIN_CLIENT):

                sys_msg.type = JOIN_CLIENT;
                strcpy(sys_msg.sender, "SERVER");
                snprintf(sys_msg.text, MAX_MSG, "%s joined the chat", msg_copy.sender);
                sem_wait(mutex);
                add_chat_history(chat, &sys_msg);
                strcpy(chat->clients[chat->client_count], msg_copy.sender);
                chat->client_count++;
                sem_post(mutex);
                break;

            // Клиент отключается
            case(LEFT_CLIENT):
                sys_msg.type = LEFT_CLIENT;
                strcpy(sys_msg.sender, "SERVER");
                snprintf(sys_msg.text, MAX_MSG, "%s left the chat", msg_copy.sender);
                sem_wait(mutex);
                add_chat_history(chat, &sys_msg);

                // Удаление клиента из списка со сдвигом
                for (int i = 0; i < chat->client_count; i++) {
                    if (strcmp(chat->clients[i], msg_copy.sender) == 0) {
                        for (int j = i; j < chat->client_count - 1; j++) {
                            strcpy(chat->clients[j], chat->clients[j+1]);
                        }
                        chat->client_count--;
                        break;
                    }
                }
                sem_post(mutex);
                break;

            // Клиент пишет сообщение
            case(TEXT_CLIENT):
                sem_wait(mutex);
                add_chat_history(chat, &msg_copy);
                sem_post(mutex);
                break;
        }
        sem_post(client_semaphore);

        // Уведомляем всех о новом сообщении. Аналог broadcast
        for (int i = 0; i < chat->client_count; i++) {
            sem_post(new_msg_sem);
        }
    }

    // Очищение ресурсов
    munmap(chat, SHM_SIZE);
    close(fd);
    sem_close(server_semaphore);
    sem_close(client_semaphore);
    sem_close(mutex);
    sem_close(new_msg_sem);
    sem_unlink(SEM_CLIENT_PATH);
    sem_unlink(SEM_SERVER_PATH);
    sem_unlink(SEM_MUTEX_PATH);
    sem_unlink(SEM_NEW_MSG_PATH);
    shm_unlink(SHM_PATH);
    exit(EXIT_SUCCESS);
}