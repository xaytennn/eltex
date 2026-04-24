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
    strcpy(message.mtext, "Hi!");
    message.mtype = 1;

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

    int send_len = msgsnd(msqid, &message, sizeof(message) - sizeof(long), 0);
    if (send_len == -1) {
        perror("msgsnd problem");
        exit(EXIT_FAILURE);
    }
    printf("Сервер отправляет сообщение! \n");


    ssize_t recv_len = msgrcv(msqid, &message, sizeof(message) - sizeof(long), 5, 0);
    if (recv_len == -1) {
        perror("msgrcv problem");
        exit(EXIT_FAILURE);
    }
    printf("Сервер получил сообщение: %s\n", message.mtext);

    if(msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl (IPC_RMID)");
        exit(1);
    }
    return 0;
}