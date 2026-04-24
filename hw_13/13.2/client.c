#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>
#include "server.h"

typedef struct {
    WINDOW *chat_win;
    WINDOW *input_win;
    WINDOW *users_win;
    char name[32];
    mqd_t server_mq;
    mqd_t client_mq;
    int running;
    pthread_mutex_t ui_mutex;
} ClientUI;

ClientUI ui;

// Принимающий поток
void *receiver_thread(void *arg) {
    struct chat_msg msg;

    while (ui.running) {
        ssize_t len = mq_receive(ui.client_mq, (char*)&msg, sizeof(msg), NULL);
        if (len > 0) {
            pthread_mutex_lock(&ui.ui_mutex);
                if (msg.type == USERS_LIST) {
                    werase(ui.users_win);
                    box(ui.users_win, 0, 0);
                    mvwprintw(ui.users_win, 0, 2, "Clients:");
                    char list_copy[MAX_MSG];
                    strcpy(list_copy, msg.text);
                    char *line = strtok(list_copy, "\n");
                    int row = 1;
                    while (line != NULL) {
                        mvwprintw(ui.users_win, row++, 1, "%s", line);
                        line = strtok(NULL, "\n");
                    }
                    wrefresh(ui.users_win);
                }
                else {
                    wprintw(ui.chat_win, "%s: %s\n", msg.sender, msg.text);
                    wrefresh(ui.chat_win);
                }
                wmove(ui.input_win, 1, 3);
                wrefresh(ui.input_win);
            pthread_mutex_unlock(&ui.ui_mutex);
        }
    }
    return NULL;
}

// Инициализация и отрисовка ncurses окон
void ncursers_window(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    int height = LINES - 3;
    ui.chat_win = newwin(height, COLS - 20, 0, 0);
    ui.users_win = newwin(height, 20, 0, COLS - 20);
    ui.input_win = newwin(3, COLS, height, 0);

    scrollok(ui.chat_win, TRUE);
    box(ui.users_win, 0, 0);
    box(ui.input_win, 0, 0);

    wrefresh(ui.chat_win);
    wrefresh(ui.users_win);
    wrefresh(ui.input_win);
}

int main (void){ 
    pthread_t recv_thread;
    struct chat_msg msg;
    struct mq_attr attr = {0, 10, sizeof(struct chat_msg), 0};
    char input[MAX_MSG];

    printf("Enter your name: ");
    fgets(ui.name, sizeof(ui.name), stdin);

    // Удаляем символ конца строки в имени
    ui.name[strcspn(ui.name, "\n")] = '\0';

    // Подключаемся к серверу через его очередь
    ui.server_mq = mq_open("/chat_queue", O_RDWR);
    if (ui.server_mq == -1) {
        perror("mq_open problem");
        exit(EXIT_FAILURE);
    }

    // Отправляем имя серверу
    msg.type = JOIN_CLIENT;
    strcpy(msg.sender, ui.name);   
    int send_len = mq_send(ui.server_mq, (char*)&msg, sizeof(msg), 0);
    if (send_len == -1) {
        perror("mq_send problem");
        exit(EXIT_FAILURE);
    }

    // Получаем номер очереди
    ssize_t recv_len = mq_receive(ui.server_mq, (char*)&msg, sizeof(msg), NULL);
    if (recv_len == -1) {
        perror("mq_receive problem");
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу через очередь клиента
    ui.client_mq = mq_open(msg.text, O_RDONLY);
    if (ui.client_mq == -1) {
        perror("mq_open problem");
        exit(EXIT_FAILURE);
    }

    // Запускаем ncurses
    ncursers_window();
    ui.running = 1;
    pthread_mutex_init(&ui.ui_mutex, NULL);
    pthread_create(&recv_thread, NULL, receiver_thread, NULL);

    while (ui.running) {
        mvwprintw(ui.input_win, 1, 1, "> ");
        wclrtoeol(ui.input_win);
        wrefresh(ui.input_win);

        echo();
        curs_set(1);
        mvwgetnstr(ui.input_win, 1, 3, input, MAX_MSG - 1);
        noecho();
        curs_set(0);

        input[strcspn(input, "\n")] = '\0';

        // Клиент выходит
        if (strcmp(input, "/exit") == 0) {
            msg.type = LEFT_CLIENT;
            strcpy(msg.sender, ui.name);
            mq_send(ui.server_mq, (char*)&msg, sizeof(msg), 0);
            ui.running = 0;
            break;
        }

        // Клиент пишет сообщение
        if (strlen(input) > 0) {
            msg.type = TEXT_CLIENT;
            strcpy(msg.sender, ui.name);
            strcpy(msg.text, input);
            mq_send(ui.server_mq, (char*)&msg, sizeof(msg), 0);
            memset(input, 0, MAX_MSG);
        }
    }

    // Освобождаем ресурсы
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    endwin();

    mq_close(ui.server_mq);
    mq_close(ui.client_mq);
    return 0;
}