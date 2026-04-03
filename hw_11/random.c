#include "random.h"
#include <time.h>
#include <pthread.h>

int my_safe_rand(int min, int max) {
    static __thread unsigned int seed = 0;
    if (seed == 0) {
        seed = time(NULL) ^ pthread_self();
    }
    seed = seed * 1103515245 + 12345;
    return min + ((seed >> 16) % (max - min + 1));
}
