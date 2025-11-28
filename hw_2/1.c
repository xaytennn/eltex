#include <stdio.h>

#define N 3

int main() {
    char arr[N][N];
    for (int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            arr[i][j] = i*N+j+1;
            // Также можно заполнить матрицу через инкремент переменной (count++;)
            // Это ускорит выполнение задачи и выглядит читабельнее
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    return 0;
}
