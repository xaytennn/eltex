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
    char *hello_string = "Hello!";
    char *received_by_share_data;
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

    int sh_memory = shmget(key_memory, SHM_SIZE, 0666);
    if (sh_memory == -1) {
        perror("shmget problem");
        exit(EXIT_FAILURE);
    }

    int semid = semget(key_semaphore, 2, 0666);
    if (semid == -1){
        perror("semget problem");
        exit(EXIT_FAILURE);
    }

    struct sembuf post_server = {SEM_SERVER, 1, 0};
    struct sembuf wait_client = {SEM_CLIENT, -1, 0};
    
    void *addr = shmat(sh_memory, NULL, 0);
    if(addr == (void *) -1){
        perror("shmat problem");
        exit(EXIT_FAILURE);
    }

    semop(semid, &wait_client, 1);
    received_by_share_data = (char *) addr;
    printf("Client received %s\n", received_by_share_data);
    strcpy(received_by_share_data, hello_string);
    printf("Client sent %s\n", received_by_share_data);
    semop(semid, &post_server, 1);

    shmdt(addr);
    return 0;
}