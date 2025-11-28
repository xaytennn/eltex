#include <stdio.h>

#define N 5

int main() {
    int mas[N];
    for (int i = 0; i < N; i++){ // Заполнение массива от 1 до N
        mas[i] = i+1;
    }
    int tmp = 0;
    for (int i = 0; i < N/2; i++){ // Своп 1 и последних переменных цикла
        tmp = mas[i];
        mas[i] = mas[N-i-1];
        mas[N-i-1] = tmp;
    }
    for (int i = 0; i < N; i++){
        printf("%d ", mas[i]);
    }
    return 0;
}
