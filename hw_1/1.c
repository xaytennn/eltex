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
    int first = 0;    
    for(int i = 31; i >= 0; i--){
        unsigned int mask = 1 << i;
    	if(n & mask) {
    	    printf("1");
    	    first = 1;
    	    
    	}
    	else if (first)
            printf("0");
    }
    printf("\n");
    return 0;
}

