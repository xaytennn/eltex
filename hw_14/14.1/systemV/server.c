#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FTOK_PATH "server"
#define SHM_SIZE 4096
#define SEM_SERVER 0
#define SEM_CLIENT 1

int main(void){
    char *hi_string = "Hi!";
    char *shared_data;
    key_t key_memory = ftok(FTOK_PATH, 5);
    if (key_memory == -1) {
        perror("ftok memory problem");
        exit(EXIT_FAILURE);
    }
    key_t key_semaphore = ftok(FTOK_PATH, 4);
    if(key_semaphore == -1){
        perror("ftok semaphore problem");
        exit(EXIT_FAILURE);
    }

    int sh_memory = shmget(key_memory, SHM_SIZE, IPC_CREAT | 0666);
    if (sh_memory == -1) {
        perror("shmget problem");
        exit(EXIT_FAILURE);
    }

    int semid = semget(key_semaphore, 2, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1){
        perror("semget problem");
        exit(EXIT_FAILURE);
    }
    struct sembuf wait_serev = {SEM_SERVER, -1, 0};
    struct sembuf post_client = {SEM_CLIENT, 1, 0};

    void *addr = shmat(sh_memory, NULL, 0);
    if(addr == (void *) -1){
        perror("shmat problem");
        exit(EXIT_FAILURE);
    }

    shared_data = (char *) addr;
    strcpy(shared_data, hi_string);
    printf("Server sent %s\n", shared_data);
    semop(semid, &post_client, 1);
    semop(semid, &wait_serev, 1);
    printf("Server received %s\n", shared_data);

    shmdt(addr);
    shmctl(sh_memory, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);    

    return 0;
}