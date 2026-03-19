#include <stdio.h>
#include "calc.h"


int main(void){

    int n = 0, a = 0, b = 0, result = 0;
    do{
        printf(
            "\n\n\nДобро пожаловать в калькулятор! Выберите желаемое действие: \n"
            "1) Сложение\n"
            "2) Вычитание\n"
            "3) Умножение\n"
            "4) Деление\n"
            "5) Выход \n"
        );

        scanf("%d", &n);
        if (n != 5){
            printf("Введите переменную a: ");
            scanf("%d", &a);
            printf("Введите переменную b: ");
            scanf("%d", &b);
        }
        switch(n){
            case 1:
                result = add(a, b);
                printf("Результат сложения: %d", result);                
            break;
            case 2:
                result = sub(a, b);
                printf("Результат вычитания: %d", result);
            break;
            case 3:
                result = mul(a, b);
                printf("Результат умножения: %d", result);
            break;
            case 4:
                result = div(a, b);
                if (b != 0) printf("Результат деления: %d", result);
            break;
            case 5:
                printf("Вы вышли из программы! \n");
                return 0;
            break;
        }
    }while(n != 5);

}
