#include <stdio.h>

#define N 3

int main() {
    char arr[N][N];
    for (int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(N-1 > i+j) arr[i][j] = 0; // Заполнение верхнего треугольника нулями
            else
                arr[i][j] = 1;
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    return 0;
}
