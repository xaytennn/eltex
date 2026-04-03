#include "loader.h"
#include "global.h"
#include "random.h"
#include <stdio.h>
#include <unistd.h>


void *load_items(void *arg){
    while(1) {
    
        // Проверка, что покупатели еще не насытились
        int all_finished = 0;
        pthread_mutex_lock(&loader_mutex);
            for(int i = 0; i < BUYER_COUNT; i++) {
                if(buyers[i].finished) {
                    all_finished++;
                }
            }	
        pthread_mutex_unlock(&loader_mutex);
        
        // Если все покупатели закрыли потребности, то выходим          
        if(all_finished == 3) {
            printf("Работа погрузчика завершена\n");
            break;
        }
        
        printf("Погрузчик проснулся\n");
        int shop_id = my_safe_rand(0, SHOP_COUNT - 1);
        Shop *shop = &shops[shop_id];
        
        // Если в магазине никого нет, то начинаем загрузку товара
        pthread_mutex_lock(&shop->mutex);
            shop->items += LOADER_ADDS;
            printf("Погрузчик добавил в %d магазин %d товара. Теперь в магазине %d товаров\n", shop_id, LOADER_ADDS, shop->items);
        pthread_mutex_unlock(&shop->mutex);
        printf("Погрузчик загрузил товар и отправился спать на 1 секунду\n");
        sleep(1);
    }
    return NULL;
}
