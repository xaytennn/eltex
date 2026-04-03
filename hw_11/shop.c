#include "shop.h"
#include "global.h"
#include "random.h"
#include <stdio.h>
#include <pthread.h>

void init_shops() {
    for (int i = 0; i < SHOP_COUNT; i++) {
        shops[i].number = i;
        shops[i].items = my_safe_rand(900, 1100);
        pthread_mutex_init(&shops[i].mutex, NULL);
        printf("Магазин %d создан, товара: %d\n", i, shops[i].items);
    }
}
