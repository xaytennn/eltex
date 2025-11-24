#include <stdio.h>

int main() {
    int n = 0;
    printf("Введите отрицательное число: ");
    scanf("%d", &n);
    if(n == 0){
        printf("0 \n");
        return 0;
    }
    if(!(n & (1 << 31))){
        printf("Ошибка. Введено положительное число! \n");
        return 0;
    }
    for(int i = 31; i >= 0; i--){
        unsigned int mask = 1 << i;
    	if(n & mask) {
    	    printf("1");    	    
    	}
    	else
            printf("0");
    }
    printf("\n");
    return 0;
}
