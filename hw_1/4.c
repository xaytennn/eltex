#include <stdio.h>

int main() {
    unsigned int n = 0;
    unsigned int newByte = 0;
    printf("Введите положительное число: ");
    scanf("%u", &n);
    printf("Введите новый байт: ");
    scanf("%u", &newByte);
    if(n == 0){
        printf("0 \n");
        return 0;
    }
    if(n & (1 << 31)){
        printf("Ошибка! Введено отрицательное число! \n");
        return 0;
    }    
    unsigned result = n & ~(0xFF << 16);
    result |= (newByte & 0xFF) << 16;

    printf("%u \n", result);
    return 0;
}
