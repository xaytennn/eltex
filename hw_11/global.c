#include "global.h"

Shop shops[SHOP_COUNT];
Buyer buyers[BUYER_COUNT];
pthread_mutex_t loader_mutex = PTHREAD_MUTEX_INITIALIZER;
