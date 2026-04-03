#include "buyer.h"
#include "global.h"
#include "random.h"
#include <stdio.h>
#include <unistd.h>

void init_buyers() {
    for (int i = 0; i < BUYER_COUNT; i++) {
        buyers[i].id = i;
        buyers[i].finished = 0;
        buyers[i].wants = my_safe_rand(9900, 10100);
        printf("Покупатель %d создан, нужно купить: %d\n", i, buyers[i].wants);
    }
}

void *buy_items(void *arg){
    
    Buyer *buyer = (Buyer*)arg;
    printf("Покупатель %d проснулся. Текущая потребность %d\n", buyer->id, buyer->wants);
    
    while(buyer->wants > 0) {
        int visited = 0;
        for(int i = 0; i < SHOP_COUNT; i++) {
        
            // Если магазин пустой, то идем дальше
            if(shops[i].items == 0) {
                continue;
            }
            
            // Если в магазине никого нет, пытаемся зайти
            if(pthread_mutex_trylock(&shops[i].mutex) == 0) {
                visited = 1;
                
                printf("Покупатель %d зашел в магазин %d. Нужно купить %d. Количество товаров магазина %d\n", buyer->id, i, buyer->wants, shops[i].items);
                // Если в магазине меньше товара, чем нужно покупателю
                if(buyer->wants >= shops[i].items) {
                    int purchase = shops[i].items;
                    buyer->wants -= purchase;
                    shops[i].items = 0;
                    printf("Покупатель %d купил в %d магазине %d товара. Осталась потребность %d\n", buyer->id, i, purchase, buyer->wants);
                    pthread_mutex_unlock(&shops[i].mutex);
                }
                
                // Если в магазине больше товара, чем нужно покупателю
                else {
                    int purchase = buyer->wants;
                    shops[i].items -= purchase;
                    buyer->wants = 0;
                    buyer->finished = 1;
                    printf("Покупатель %d купил в %d магазине %d товара. Потребностей не осталось!\n", buyer->id, i, purchase);
                    pthread_mutex_unlock(&shops[i].mutex);
                }
            }
            
            // Потребность покупателя удовлетворена. Выходим
            if(buyer->wants == 0){
                buyer->finished = 1;
                break;
            }
            
            // Покупатель сходил в магазин, но не удовлетворил потребность
            if(buyer->wants > 0 && visited) {
                printf("Покупатель %d закупился в магазине %d, но этого было недостаточно. Отправляется спать на 2 секунды\n", buyer->id, i);
                sleep(2);
                break;
            }
        }
    }
    return 0;
}
