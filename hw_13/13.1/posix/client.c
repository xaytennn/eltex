#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (void){ 
    mqd_t mq, mq2;
    char msg[] = "Hello!";
    char rd_msg[8192];
    mq = mq_open("/posix_queue", O_RDWR);
    mq2 = mq_open("/posix_queue2", O_RDWR);
    if(mq == -1 || mq2 == -1){
        perror("mq_open problem");
        exit(EXIT_FAILURE);
    }

    ssize_t recv_len = mq_receive(mq, rd_msg, sizeof(rd_msg), NULL);
    if(recv_len == -1) {
        perror("mq_receive problem");
        exit(EXIT_FAILURE);
    }
    printf("Клиент получил сообщение: %s\n", rd_msg);

    int send_len = mq_send(mq2, msg, strlen(msg) + 1, 2);
    if(send_len == -1) {
        perror("mq_send problem");
        exit(EXIT_FAILURE);
    }
    printf("Клиент отправил сообщение\n");
    mq_close(mq);
    mq_close(mq2);
    return 0;
}