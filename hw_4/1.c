#include <stdio.h>

#define N 100

struct abonent {
    char name[10];
    char second_name[10];
    char tel[10];
};

struct abonent phoneBook[N];
int count = 0;

// Функция, сравнивающая две строки. По аналогии с strcmp
int my_strcmp(char *a, char *b) {
    while (*a && *b) {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

// Функция для удаления '\n' из fgets
void fix_fgets(char *s, int size) {
    for (int i = 0; i < size; i++) {
        if (s[i] == '\n') {
            s[i] = '\0';
            return;
        }    
    }
    // Очищаем stdin, если введено ровно лимитированное количество символов т.к. fgets оставляет '\n' в stdin
    int c;
    while ((c = getchar()) != '\n' && c != EOF);    
}

// Фунция для добавления абонента
void add_abonent(){
    // Проверка, что справочник не заполнен
    if (count >= N){
        printf("Справочник переполнен \n");
        return;
    }
    
    for(int i = 0; i < N; i++){
        if(phoneBook[i].name[0] == '\0'){
            printf("Введите имя: \n");
            fgets(phoneBook[i].name, 10, stdin);
            fix_fgets(phoneBook[i].name, 10);

            printf("Введите фамилию: \n");
            fgets(phoneBook[i].second_name, 10, stdin);
            fix_fgets(phoneBook[i].second_name, 10);
            
            printf("Введите телефон: \n");
            fgets(phoneBook[i].tel, 10, stdin);
            fix_fgets(phoneBook[i].tel, 10);
            
            count++;
            printf("Абонент добавлен \n");
            return;
        }
    }
}

// Функция для удаления абонента из справочника
void delete_abonent(){
    char name[10], second_name[10];
    printf("Введите имя, удаляемого абонента: \n");
    fgets(name, 10, stdin);
    fix_fgets(name, 10);
            
    printf("Введите фамилию, удаляемого абонента: \n");
    fgets(second_name, 10, stdin);
    fix_fgets(second_name, 10);

    // Пробегаем весь массив, чтобы найти подходящих абонентов
    for (int i = 0; i < N; i++) { 
    
    // Если нашли подходящего, проверяет полное совпадение имени и фамилии
        if (my_strcmp(phoneBook[i].name, name) &&
            my_strcmp(phoneBook[i].second_name, second_name)) {
            
            // Зануляем каждый элемент массива посимвольно
            for (int j = 0; j < 10; j++) {
                phoneBook[i].name[j] = '\0';
                phoneBook[i].second_name[j] = '\0';
                phoneBook[i].tel[j] = '\0';            
            }
            count--;
            printf("Абонент удалён \n");
            return;
        }
    }
}

// Функция для поиска абонента по имени
void search_by_name(){
    char name[10];
    int found = 0;

    printf("Введите имя для поиска: ");
    fgets(name, 10, stdin);
    fix_fgets(name, 10);
    
    // Ищем хотя бы одно совпадение
    for (int i = 0; i < N; i++) {
        if (my_strcmp(phoneBook[i].name, name)) {
            printf("%s %s %s\n", phoneBook[i].name, phoneBook[i].second_name, phoneBook[i].tel);
            found = 1;
        }
    }

    if (!found)
        printf("Абонент с таким именем не найден \n");
}

// Функция для вывода всех абонентов
void print_all(){
    for (int i = 0; i < N; i++){
        if(phoneBook[i].name[0] == '\0')
            return;
        printf("%s %s %s\n", phoneBook[i].name, phoneBook[i].second_name, phoneBook[i].tel);
    }
}

int main(void){
    char buf[10];
    int n = 0;
    do{
        printf(
            "\n\n\nВыберите желаемое действие: \n"
            "1) Добавить абонента\n"
            "2) Удалить абонента\n"
            "3) Поиск абонентов по имени\n"
            "4) Вывод всех записей\n"
            "5) Выход \n"
        );
        
        fgets(buf, 10, stdin);
        n = buf[0] - '0';
        switch(n){
            case 1:
                add_abonent();
            break;
            case 2:
                delete_abonent();
            break;        
            case 3:
                search_by_name();
            break;
            case 4:
                print_all();
            break;
            case 5:
                printf("Вы вышли из программы! \n");
                return 0;
            break; 
        }
    }while(n != 5);

}
