#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define MAX_DRIVERS 4 // Максимальное количество водителей. Память выделяется как 2*current_memory
#define MAX_BUFF_SIZE 10 // Максимальный размер буфера для передачи через pipe водителю

typedef struct {
    pid_t driver_pid; // PID процесса
    int pipe_fd; // Дескриптор для чтения
    int busy; // Проверка статуса занятости
    time_t busy_time; // Время, на которое занят водитель
} Driver;

void work_driver(int pipe_fd);

int running = 1; // Флаг работы программы
int current_drivers = 0; // Текущее количество водителей

Driver *create_driver(){
    Driver *driver = malloc(sizeof(Driver));
    int pipe_fd[2];
    pipe(pipe_fd);
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork create_driver error");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0) {
        // Потомок
        close(pipe_fd[1]); // Закрываем дескриптор на запись
        work_driver(pipe_fd[0]); // Передает дескриптор на чтение
        exit(0);
    }
    else if(pid > 0){
        // Родитель
        close(pipe_fd[0]);
        driver->driver_pid = pid;
        driver->pipe_fd = pipe_fd[1];
        driver->busy = 0;
        driver->busy_time = 0;
        return driver;
    }
    return NULL;
}

// Поиск водителя по pid. Если не найден -- возвращает NULL
Driver *search_driver(Driver **drivers, pid_t pid){
    for(int i = 0; i < current_drivers; i++){
        if(drivers[i]->driver_pid == pid) return drivers[i];
    }
    return NULL;
}

void send_task(Driver *driver, int task_timer){
    // Родительский процесс проверяет, свободен ли водитель
    if(!driver->busy){
        char buffer[MAX_BUFF_SIZE];
        // Записывает задание для водителя в pipe
        snprintf(buffer, sizeof(buffer), "task %d\n", task_timer);
        driver->busy_time = task_timer + time(NULL);
        driver->busy = 1;
        write(driver->pipe_fd, buffer, strlen(buffer));
    }
    else{
        printf("Try again! Driver %d is busy on %ld sec\n", driver->driver_pid, driver->busy_time-time(NULL));
    }
}

void work_driver(int pipe_fd){ 
    char buffer[MAX_BUFF_SIZE];
    ssize_t n;
    int busy = 0;
    int task_time = 0;
    time_t busy_time = 0;
    
    // Проверяем события POLLIN в дескрипторах с помощью poll
    struct pollfd poll_fd;
    poll_fd.fd = pipe_fd;
    poll_fd.events = POLLIN;

    while(running){
        int ret = poll(&poll_fd, 1, 100);
        if(ret == -1){
            perror("poll work_driver error");
            close(pipe_fd);
            exit(EXIT_FAILURE);
        }
        
        // Считаем через таймер время работы водителя
        if(busy && busy_time-time(NULL) <= 0){
            busy = 0;
        }
        
        if(ret > 0 && (poll_fd.revents & POLLIN)){
            n = read(pipe_fd, buffer, sizeof(buffer));
            if(n <= 0){
                perror("read work_driver error");
                close(pipe_fd);
                exit(EXIT_FAILURE);
            }
            buffer[n] = '\0';
            // Принимаемся за задание водителю
            if((strncmp(buffer, "task", 4) == 0) && (busy == 0)){
                if(strlen(buffer) > 6) task_time = atoi(buffer + 5);
                busy = 1;
                busy_time = time(NULL) + task_time;
                printf("Driver %d is busy on %d sec\n", getpid(), task_time);
            }
        }   
    }
}

// Родительский процесс проверяет статус водителя
void get_status(Driver *driver){
    if(driver->busy_time-time(NULL) <= 0) driver->busy = 0;
    if(driver->busy == 1) printf("Driver %d is busy on %ld sec\n", driver->driver_pid, driver->busy_time-time(NULL));
    else printf("Driver %d is available\n", driver->driver_pid);
}

// CLI программы 
void init_cli_interface(){
    printf("\tChoose what you want to do: \n"
        "\t1. Create driver\n"
        "\t2. Send task: <pid> <task timer>\n"
        "\t3. Get driver status: <pid>\n"
        "\t4. Get all drivers status\n"
        "\t5. Exit\n");
}

// Выключаем сервер, если поймали сигнал
void safe_close_app(int sig) {
    running = 0; 
}

void drivers_free(Driver **drivers){
    int status;
    for(int i = 0; i < current_drivers; i++){
        kill(drivers[i]->driver_pid, SIGKILL);
        waitpid(drivers[i]->driver_pid, &status, 0);
        close(drivers[i]->pipe_fd);
        free(drivers[i]);
    }
}

int main(void){

    // Перехватываем сигналы завершения, чтобы не плодить зомби процессы
    struct sigaction handler = {0};
    handler.sa_handler = safe_close_app;
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);
    sigaction(SIGTERM, &handler, NULL);

    Driver **drivers = NULL;
    int capacity_drivers = 0;
    pid_t pid_task;
    
    int time_task;
    int choose;
    Driver *need_driver = NULL;
    while(running){
        init_cli_interface();
        scanf("%d", &choose);
        
        switch(choose){
            case 1:
                if(current_drivers >= MAX_DRIVERS){
                    printf("Warning! To much drivers. You can't create one more\n");
                    break;
                }
                if(capacity_drivers == 0) {
                    capacity_drivers = 2;
                    drivers = realloc(drivers, capacity_drivers * sizeof(Driver*));
                }
                if(current_drivers + 1 == capacity_drivers) {
                    capacity_drivers *= 2;
                    drivers = realloc(drivers, capacity_drivers * sizeof(Driver*));
                }
                if(!drivers){
                    perror("realloc case 1 error");
                }
                drivers[current_drivers] = create_driver();
                printf("PID: %d\n", drivers[current_drivers]->driver_pid);
                current_drivers++;
                break;
            case 2:
                printf("Input pid and time task\n");
                scanf("%d %d", &pid_task, &time_task);
                need_driver = search_driver(drivers, pid_task);
                if(need_driver != NULL) send_task(need_driver, time_task);
                break;
            case 3:
                printf("Input pid\n");
                scanf("%d", &pid_task);
                need_driver = search_driver(drivers, pid_task);
                if(need_driver != NULL) get_status(need_driver);
                break;
            case 4:
                for(int i = 0; i < current_drivers; i++){
                    get_status(drivers[i]);
                }
                break;
            case 5:
                drivers_free(drivers);
                exit(EXIT_SUCCESS);
                break;
        }
    }
    if(!running) drivers_free(drivers);
    exit(EXIT_SUCCESS);
}