#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SHM_PATH "/shared_data"
#define SHM_SIZE 4096
#define SEM_SERVER_PATH "/server_semaphore_name"
#define SEM_CLIENT_PATH "/client_semaphore_name"

int main(void){
    char *received_by_share_data;
    char *hello_string = "Hello!";
    sem_t *server_semaphore = sem_open(SEM_SERVER_PATH, 0);
    if(server_semaphore == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    sem_t *client_semaphore = sem_open(SEM_CLIENT_PATH, 0);
    if(client_semaphore == SEM_FAILED){
        perror("sem_open problem");
        exit(EXIT_FAILURE);
    }
    int fd = shm_open(SHM_PATH, O_RDWR, 0666);
    if(fd == -1){
        perror("shm_open problem");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap problem");
        exit(EXIT_FAILURE);
    }

    sem_wait(client_semaphore);
    received_by_share_data = (char *) addr;
    printf("Client received %s\n", received_by_share_data);
    strcpy(received_by_share_data, hello_string);
    printf("Client sent %s\n", hello_string);
    sem_post(server_semaphore);

    close(fd);
    munmap(addr, SHM_SIZE);
    sem_close(server_semaphore);
    sem_close(client_semaphore);
    return 0;
}