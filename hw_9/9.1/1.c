#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(void){
    char ch;
    
    // Создание файла и запись
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Ошибка при создании(открытии) файла для записи");
        return 1;
    }
    
    if(write(fd, "String from file", strlen("String from file")) < 0){
        perror("Ошибка при записи в файл");
        close(fd);
        return 1;
    }
    close(fd);
    
    // Открываем файл на чтение
    fd = open("output.txt", O_RDONLY);
    if (fd == -1) {
        perror("Ошибка при открытии файла для чтения");
        return 1;
    }
    
    // Определяем размер строки
    off_t str_size = lseek(fd, 0, SEEK_END);
    
    // Чтение из файла с конца и вывод строки посимвольно
    for(int i = str_size - 1; i >= 0; i--){
        lseek(fd, i, SEEK_SET);
        if (read(fd, &ch, 1) < 0) {
            perror("Ошибка при чтении символа с конца строки");
            close(fd);
            return 1;
        }
        printf("%c", ch);
    }
    printf("\n");
    close(fd);
    return 0;
}
