#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "global.h"
#include "random.h"
#include "shop.h"
#include "buyer.h"
#include "loader.h"


int main(void) {
    srand(time(NULL));
    int i;
    int *buyer_status, *loader_status;
    
    init_shops();
    init_buyers();
    
    pthread_t buyers_thread[BUYER_COUNT];
    pthread_t loader_thread;
    
    
    for(i = 0; i < BUYER_COUNT; i++) {
        pthread_create(&buyers_thread[i], NULL, buy_items, &buyers[i]);
    }
    
    pthread_create(&loader_thread, NULL, load_items, NULL);

    
    for(i = 0; i < BUYER_COUNT; i++) {
        pthread_join(buyers_thread[i], (void **) &buyer_status);
    }
    
    pthread_join(loader_thread, (void **) &loader_status);
    printf("Все покупатели закрыли свои потребности. Программа завершена! \n");
    return 0;
}
