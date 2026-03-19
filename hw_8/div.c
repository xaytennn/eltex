#include <stdio.h>
#include "calc.h"

int div(int a, int b){
	if (b == 0) {
	    printf("Ошибка! Деление на 0 запрещено!\n");
	    return 0;
	}
	return a / b;
}
