#ifndef GLOBAL_H
#define GLOBAL_H

#include <pthread.h>

#define BUYER_COUNT 3
#define SHOP_COUNT 5
#define LOADER_ADDS 200

typedef struct Shop {
    int items;
    int number;
    pthread_mutex_t mutex;
} Shop;

typedef struct Buyer {
    int wants;
    int id;
    int finished;
} Buyer;

extern Shop shops[SHOP_COUNT];
extern Buyer buyers[BUYER_COUNT];
extern pthread_mutex_t loader_mutex;

int my_safe_rand(int min, int max);

#endif

