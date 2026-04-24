#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (void){
    mqd_t mq, mq2;
    char msg[] = "Hi!";
    char rd_msg[256];
    struct mq_attr attr = {0, 10, 256, 0};
    mq_unlink("/posix_queue");
    mq_unlink("/posix_queue2");
    mq = mq_open("/posix_queue", O_CREAT  | O_RDWR, 0666, &attr);
    mq2 = mq_open("/posix_queue2", O_CREAT  | O_RDWR, 0666, &attr);
    if(mq == -1 || mq2 == -1){
        perror("mq_open problem");
        exit(EXIT_FAILURE);
    }

    int send_len = mq_send(mq, msg, strlen(msg) + 1, 1);
    if(send_len == -1) {
        perror("mq_send problem");
        exit(EXIT_FAILURE);
    }
    printf("Сервер отправил сообщение\n");
    unsigned int prio;
    ssize_t len = mq_receive(mq2, rd_msg, sizeof(rd_msg), &prio);
    if (len == -1) {
        perror("mq_receive problem");
        exit(EXIT_FAILURE);
    }


    printf("Сервер получил сообщение: %s\n", rd_msg);

    mq_close(mq);
    mq_close(mq2);
    mq_unlink("/posix_queue");
    mq_unlink("/posix_queue2");
    return 0;
}