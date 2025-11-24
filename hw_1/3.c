#include <stdio.h>

int main() {
    unsigned int n = 0;
    printf("Введите положительное число: ");
    scanf("%u", &n);
    if(n == 0){
        printf("0 \n");
        return 0;
    }
    if(n & (1 << 31)){
        printf("Ошибка! Введено отрицательное число! \n");
        return 0;
    }
    int count = 0;    
    for(int i = 31; i >= 0; i--){
        unsigned int mask = 1 << i;
    	if(n & mask)
    	    count++;
    }	
    printf("Количество единиц в двоичном представлении = %d\n", count);
    return 0;
}
