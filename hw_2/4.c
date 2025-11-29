#include <stdio.h>

#define N 5

int main() {
    int arr[N][N];
    int top = 0;     // Граница сверху
    int left = 0;    // Граница слева
    int bot = N-1;   // Граница снизу
    int right = N-1; // Граница справа
    int count = 1;
    
    while (count <= N*N){
        for (int i = left; i <= right; i++){ // Заполняем до правой границы (строка)
            arr[top][i] = count++;
        }
        top++; // Убираем верхнюю строку т.к. её уже заполнили
        for (int i = top; i <= bot; i++){ // Заполняем до нижней границы (столбец)
            arr[i][right] = count++;
        }
        right--; // Убираем правый столбец
        for (int i = right; i >= left; i--){ // Заполняем до левой границы (строка)
            arr[bot][i] = count++;
        }
        bot--; // Убираем нижнюю строчку
        for (int i = bot; i >= top; i--){ // Заполняем до верхней границы (столбец)
            arr[i][left] = count++;
        }
        left++; // Убираем левый столбец
        
    }
    
    for (int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    return 0;
}
