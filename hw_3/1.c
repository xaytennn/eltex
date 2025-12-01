#include <stdio.h>


int main(void){
    int n = 0, newByte = 0;
    char *p = (char *)&n;
    
    printf("Введите число n = ");
    scanf("%d", &n);
    printf("Введите новый третий байт для замены = ");
    scanf("%d", &newByte);
    
    p+=2; // Ставим указатель на третий байт
    *p = newByte; // Заменяем третий байт
    
    printf("Новое число n = %d\n", n);
}
