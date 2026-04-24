#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct msgbuf {
    long mtype;
    char mtext[256];
};

int main(void){
    key_t key;
    int msqid;
    struct msgbuf message;

    key = ftok("server.c", 0);
    if (key == -1) {
        perror("ftok problem");
        exit(EXIT_FAILURE);
    }

    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget problem");
        exit(EXIT_FAILURE);
    }

    ssize_t recv_len = msgrcv(msqid, &message, sizeof(message) - sizeof(long), 1, 0);
    if (recv_len == -1) {
        perror("msgrcv problem");
        exit(EXIT_FAILURE);
    }

    printf("Клиент получил сообщение: %s\n", message.mtext);

    message.mtype = 5;
    strcpy(message.mtext, "Hello!");
    printf("Клиент отправляет сообщение! \n");
    int send_len = msgsnd(msqid, &message, sizeof(message) - sizeof(long), 0);
    if (send_len == -1) {
        perror("msgsnd problem");
        exit(EXIT_FAILURE);
    }

    return 0;
}