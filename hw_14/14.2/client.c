#include "chat_defs.h"
#include <ncurses.h>

chat_t *chat;
sem_t *server_semaphore, *client_semaphore, *mutex, *new_msg_sem;
int running = 1;
int last_hist_count = 0;
WINDOW *chat_win, *input_win, *users_win;;

// Вывод новых сообщений из истории
void print_new_messages(void) {
    sem_wait(mutex);

    if (last_hist_count >= MAX_HISTORY || last_hist_count > chat->hist_count) {
        last_hist_count = 0;
    }
    while (last_hist_count < chat->hist_count) {
        int pos;
        if (chat->hist_count < MAX_HISTORY) {
            pos = last_hist_count;
        }
        else {
            pos = (chat->hist_index + last_hist_count - MAX_HISTORY + MAX_HISTORY) % MAX_HISTORY;
        }
        chat_msg_t *msg = &chat->history[pos];
        wmove(chat_win, getcury(chat_win), 1);
        wprintw(chat_win, "%s: %s\n", msg->sender, msg->text);
        last_hist_count++;
    }
    wrefresh(chat_win);
    sem_post(mutex);
}

// Запуск и отрисовка окон ncurses 
void init_ncurses(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    int h, w;
    getmaxyx(stdscr, h, w);

    chat_win = newwin(h - 3, w - 20, 0, 0);
    users_win = newwin(h - 3, 20, 0, w - 20);
    input_win = newwin(3, w, h - 3, 0);

    scrollok(chat_win, TRUE);
    box(users_win, 0, 0);
    mvwprintw(users_win, 0, 2, "Clients:");
    box(input_win, 0, 0);

    wrefresh(chat_win);
    wrefresh(users_win);
    wrefresh(input_win);
}

// Обновление блока активных клиентов
void update_users_list(void) {
    sem_wait(mutex);
    werase(users_win);
    box(users_win, 0, 0);
    mvwprintw(users_win, 0, 2, "Clients:");
    for (int i = 0; i < chat->client_count; i++) {
        mvwprintw(users_win, i + 1, 1, "- %s", chat->clients[i]);
    }
    wmove(input_win, 1, 3);
    wrefresh(users_win);
    sem_post(mutex);
}

// Поток, принимающий сообщения
void *receiver_thread(void *arg) {
    while (running) {
        sem_wait(new_msg_sem);
        if (!running) break;

        print_new_messages();
        update_users_list();
    }
    return NULL;
}

int main(void){
    chat_msg_t user;
    pthread_t client_thread;
    char input[MAX_MSG];

    // Подключаем 4 семафора:
    // Сервер, клиент, аналог мьютекса и семафор для проверки новых сообщений
    server_semaphore = sem_open(SEM_SERVER_PATH, 0);
    if(server_semaphore == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    client_semaphore = sem_open(SEM_CLIENT_PATH, 0);
    if(client_semaphore == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    mutex = sem_open(SEM_MUTEX_PATH, 0);
    if(mutex == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    new_msg_sem = sem_open(SEM_NEW_MSG_PATH, 0);
    if(new_msg_sem == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }

    int fd = shm_open(SHM_PATH, O_RDWR, 0666);
    if(fd == -1){
        perror("shm_open problem");
        exit(EXIT_FAILURE);
    }

    init_ncurses();

    chat = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(chat == MAP_FAILED){
        perror("mmap problem");
        exit(EXIT_FAILURE);
    }

    // Считываем имя клиента при подключении и записываем в shared memory
    mvwprintw(input_win, 1, 1, "Enter your name: ");
    wrefresh(input_win);
    echo();
    curs_set(1);
    mvwgetnstr(input_win, 1, 18, user.sender, MAX_NAME - 1);
    noecho();
    curs_set(0);
    user.sender[strcspn(user.sender, "\n")] = '\0';

    // Запуск потока клиентов
    pthread_create(&client_thread, NULL, receiver_thread, NULL);

    sem_wait(mutex);
    chat->new_msg.type = JOIN_CLIENT;
    strcpy(chat->new_msg.sender, user.sender);
    sem_post(mutex);
    sem_post(server_semaphore);
    sem_wait(client_semaphore);

    while(1){

        // Считываем, что ввел клиент
        werase(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "> ");
        wrefresh(input_win);

        echo();
        curs_set(1);
        mvwgetnstr(input_win, 1, 3, input, MAX_MSG - 1);
        noecho();
        curs_set(0);
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0) continue;

        // Отключаемся, если встретим /exit
        if (strcmp(input, "/exit") == 0){
            sem_wait(mutex);
            chat->new_msg.type = LEFT_CLIENT;
            strcpy(chat->new_msg.sender, user.sender);
            sem_post(mutex);
            sem_post(server_semaphore);
            sem_wait(client_semaphore);
            break;
        }
        else{
            // Обрабатываем остальной текст
            sem_wait(mutex);
            chat->new_msg.type = TEXT_CLIENT;
            strcpy(chat->new_msg.sender, user.sender);
            strcpy(chat->new_msg.text, input);
            sem_post(mutex);
            sem_post(server_semaphore);
            sem_wait(client_semaphore);
        }
    }

    // Закрываем все, что использовали
    pthread_cancel(client_thread);
    pthread_join(client_thread, NULL);
    close(fd);
    endwin();
    munmap(chat, SHM_SIZE);
    sem_close(server_semaphore);
    sem_close(client_semaphore);
    sem_close(new_msg_sem);
    sem_close(mutex);
    exit(EXIT_SUCCESS);
}