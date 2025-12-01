#include <stdio.h>

#define N 10

int main() {
    int mas[N];
    for (int i = 0; i < N; i++){ // Заполнение массива от 1 до N
        mas[i] = i+1;
    }
    
    int *pmas = mas; // Указатель на mas[0]
    for (int i = 0; i < N; i++){
        printf("%d ", *(pmas+i)); // Вывод массива
    }
    return 0;
}
