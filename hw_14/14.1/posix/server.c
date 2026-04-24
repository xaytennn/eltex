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
    char *shared_data;
    char *hi_string = "Hi!";

    shm_unlink(SHM_PATH);
    sem_unlink(SEM_SERVER_PATH);
    sem_unlink(SEM_CLIENT_PATH);

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

    int fd = shm_open(SHM_PATH, O_CREAT | O_RDWR | O_EXCL, 0666);
    if(fd == -1){
        perror("shm_open problem");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(fd, SHM_SIZE) == -1){
        perror("ftruncate problem");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap problem");
        exit(EXIT_FAILURE);
    }

    sem_wait(server_semaphore);

    shared_data = (char *) addr;
    strcpy(shared_data, hi_string);
    printf("Server sent %s\n", shared_data);
    sem_post(client_semaphore);
    sem_wait(server_semaphore);
    printf("Server received %s\n", shared_data);



    munmap(addr, SHM_SIZE);
    close(fd);
    sem_close(server_semaphore);
    sem_close(client_semaphore);
    sem_unlink(SEM_CLIENT_PATH);
    sem_unlink(SEM_SERVER_PATH);
    shm_unlink(SHM_PATH);
    return 0;
}