#ifndef CHAT_DEFS_H
#define CHAT_DEFS_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#define SHM_PATH "/shared_data"
#define SHM_SIZE 65536
#define SEM_SERVER_PATH "/server_semaphore_name"
#define SEM_CLIENT_PATH "/client_semaphore_name"
#define SEM_MUTEX_PATH "/mutex_semaphore_name"
#define SEM_NEW_MSG_PATH "/new_msg_semaphore_name"
#define JOIN_CLIENT 1
#define LEFT_CLIENT 2
#define TEXT_CLIENT 3
#define MAX_HISTORY 50
#define MAX_CLIENTS 10
#define MAX_MSG 256
#define MAX_NAME 32

typedef struct {
    int type;
    char sender[MAX_NAME];
    char text[MAX_MSG];
} chat_msg_t;

typedef struct {
    int hist_index;
    int hist_count;
    int client_count;
    chat_msg_t history[MAX_HISTORY];
    chat_msg_t new_msg;
    char clients[MAX_CLIENTS][MAX_NAME];
} chat_t;

#endif