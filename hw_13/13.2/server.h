#ifndef SERVER_H
#define SERVER_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_HISTORY 50
#define MAX_CLIENTS 10
#define MAX_MSG 256
#define JOIN_CLIENT 1
#define LEFT_CLIENT 2
#define TEXT_CLIENT 3
#define USERS_LIST 4

struct chat_msg{
    int type;
    char text[MAX_MSG];
    char sender[32];
} chat_msg;

typedef struct Client{
    int active;
    mqd_t mq;
    char qname[64];
    char name[32];

} Client;

int hist_count = 0;
int hist_index = 0;
struct chat_msg history[MAX_HISTORY];

void broadcast(struct chat_msg *msg);
void add_chat_history(struct chat_msg *msg);
void send_history_to_client(mqd_t client_mq);
void send_users_list_to_all();

#endif