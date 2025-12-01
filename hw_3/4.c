#include <stdio.h>

int main() {
    char buffer[256];
    char substr[255];
    
    printf("Введите строку: ");
    scanf("%s", buffer);
    printf("\nВведите подстроку для поиска: ");
    scanf("%s", substr);
    
    char *presult = NULL; // Указатель на начало найденной подстроки
    
    for(char *p = buffer; *p != '\0'; p++){ // Поиск от начала строки до нулевого символа (конца строки)
        char *pbuffer = p;
        char *psubstr = substr;
        
        // Цикл, пока не закончилась строка или подстрока
        // Посимвольное сравнение
        while (*pbuffer != '\0' && *psubstr != '\0' && *pbuffer == *psubstr){
            pbuffer++;
            psubstr++;
        }
        
        // Если подстрока закончилась до выхода из цикла, то передаем результат
        if(*psubstr == '\0'){
            presult = p;
            printf("Подстрока найдена, начало: %s\n", presult);
            return 0;
        }
    }    
    
    printf("Подстрока не найдена");
    return 0;
}
