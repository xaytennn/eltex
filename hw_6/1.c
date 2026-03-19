#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100

#define MAX_NAME 10
#define MAX_SECOND_NAME 10
#define MAX_TEL 10

// Структура абонента
typedef struct Abonent {
    char name[MAX_NAME];
    char second_name[MAX_SECOND_NAME];
    char tel[MAX_TEL];
} Abonent;

// Структура узла двусвязного списка
typedef struct Node {
    Abonent data;
    struct Node* prev;
    struct Node* next;
} Node;

// Структура для управления списком
typedef struct PhoneBook{
    Node* head;
    Node* tail;
    int count;
} PhoneBook;

// Инициализация пустого справочника
PhoneBook* createPhoneBook() {
    PhoneBook* book = (PhoneBook*)malloc(sizeof(PhoneBook));
    if (book == NULL){
    	perror("Ошибка, не выделилась память на справочник\n");
    	return NULL;
    }
    book->head = NULL;
    book->tail = NULL;
    book->count = 0;
    return book;
}

// Создание нового узла
Node* createNode(Abonent abonent) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL){
    	perror("Ошибка, не выделилась память на узел\n");
    	return NULL;
    }
    newNode->data = abonent;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

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
void add_abonent(PhoneBook* book){
    if (book == NULL) {
        printf("Ошибка, справочник не создан\n");
        return;
    }


    Abonent newAbonent;

    printf("\n\n\nВведите имя: \n");
    fgets(newAbonent.name, MAX_NAME, stdin);
    fix_fgets(newAbonent.name, MAX_NAME);

    printf("Введите фамилию: \n");
    fgets(newAbonent.second_name, MAX_SECOND_NAME, stdin);
    fix_fgets(newAbonent.second_name, MAX_SECOND_NAME);
         
    printf("Введите телефон: \n");
    fgets(newAbonent.tel, MAX_TEL, stdin);
    fix_fgets(newAbonent.tel, MAX_TEL);
    
    // Создание нового узла
    Node* newNode = createNode(newAbonent);
    if (newNode == NULL) {
        printf("Ошибка, узел не создан\n");
        return;
    }    
    
    // Если это первый элемент
    if(book->head == NULL){
        book->head = newNode;
        book->tail = newNode;
    } else {
        // Добавляем в конец списка
        newNode->prev = book->tail;
        book->tail->next = newNode;
        book->tail = newNode;
    }
    
    book->count++;       
    printf("\n\n\nАбонент добавлен \n\n\n");
}

// Функция для удаления абонента из справочника
void delete_abonent(PhoneBook* book){
    if (book == NULL) {
        printf("Ошибка, справочник не создан\n");
        return;
    }
    
    char name[MAX_NAME], second_name[MAX_SECOND_NAME];
    printf("\n\n\nВведите имя, удаляемого абонента: \n");
    fgets(name, MAX_NAME, stdin);
    fix_fgets(name, MAX_NAME);
            
    printf("Введите фамилию, удаляемого абонента: \n");
    fgets(second_name, MAX_SECOND_NAME, stdin);
    fix_fgets(second_name, MAX_SECOND_NAME);

    // Пробегаем весь массив, чтобы найти подходящих абонентов
    Node* found = book->head;
    
        while (found != NULL) { 
    
    // Если нашли подходящего, проверяет полное совпадение имени и фамилии
        if (my_strcmp(found->data.name, name) &&
            my_strcmp(found->data.second_name, second_name)) {
            
            // Удаляем абонента, если он единственный в списке
            if(found == book->head && found == book->tail){
            	book->head = NULL;
            	book->tail = NULL;
            }
            // Удаляем абонента, если он первый в списке
            else if (found == book->head){
            	book->head = found->next;
            	book->head->prev = NULL;
            }
            // Удаляем абонента, если он последний в списке
            else if (found == book->tail){
            	book->tail = found->prev;
            	book->tail->next = NULL;
            }
            // Удаляем абонента в середине списка
            else{
            	found->prev->next = found->next;
            	found->next->prev = found->prev;
            }
            
            // Освобождаем из памяти удаленный элемент
            free(found);
            book->count--;
            printf("Абонент удалён \n");
            return;
        }
        found = found->next;
    }
}

// Функция для поиска абонента по имени
void search_by_name(PhoneBook* book){
    if (book == NULL) {
        printf("Ошибка, справочник не создан\n");
        return;
    }
    char name[MAX_NAME];
    int count = 0, index = 0;
    if (book->head == NULL) {
        printf("\n\n\nСправочник абонентов пуст!\n\n\n");
        return;
    }
    
    printf("\n\n\nВведите имя абонента для поиска: \n");
    fgets(name, MAX_NAME, stdin);
    fix_fgets(name, MAX_NAME);

    Node* found = book->head;
    while (found != NULL) { 
    	if (my_strcmp(found->data.name, name)) {
            printf("\n\nАбонент № %d\n", index);
            printf("Имя абонента: %s\n", found->data.name);
            printf("Фамилия абонента: %s\n", found->data.second_name);
            printf("Телефон абонента: %s\n\n", found->data.tel);
            count = 1;
    	}
    	index++;
    	found = found->next;
    }

    if (!count)
        printf("\n\n\nАбонент с таким именем не найден \n\n\n");
}

// Функция для вывода всех абонентов
void print_all(PhoneBook* book){
    if (book == NULL) {
        printf("Ошибка, справочник не создан\n");
        return;
    }
    int index = 0;
    if (book->head == NULL) {
        printf("\n\n\nСправочник абонентов пуст!\n\n\n");
        return;
    }
    Node* found = book->head;
    while (found != NULL) { 
        printf("\n\nАбонент № %d\n", index);
        printf("Имя абонента: %s\n", found->data.name);
        printf("Фамилия абонента: %s\n", found->data.second_name);
        printf("Телефон абонента: %s\n\n", found->data.tel);
    	index++;
    	found = found->next;
    }
}

// Функция для очищения памяти
void free_memory(PhoneBook* book){
    if (book == NULL) {
        printf("Ошибка, справочник не создан\n");
        return;
    }
    Node* found = book->head;
    
    // Освобождаем каждый узел
    while (found != NULL) { 
        Node* tmp = found;	
    	found = found->next;
    	free(tmp);
    }
    book->head = NULL;
    book->tail = NULL;
    book->count = 0;
    free(book);
    printf("\nОчищение памяти завершено! \n\n"); 
}

int main(void){
    PhoneBook* book = createPhoneBook();
    if (book == NULL) {
        printf("Ошибка, справочник не создан!\n");
        exit(EXIT_FAILURE);
    }    
    char buf[MAX_SECOND_NAME];
    int n = 0;
    do{
        printf(
            "\n\n\nВыберите желаемое действие: \n"
            "1) Добавить абонента\n"
            "2) Удалить абонента\n"
            "3) Поиск абонентов по имени\n"
            "4) Вывод всех записей\n"
            "5) Очищение памяти\n"
            "6) Выход \n"
        );
        
        fgets(buf, sizeof(buf), stdin);
        n = buf[0] - '0';
        switch(n){
            case 1:
                add_abonent(book);
            break;
            case 2:
                delete_abonent(book);
            break;        
            case 3:
                search_by_name(book);
            break;
            case 4:
                print_all(book);
            break;
            case 5:
                printf("\n\n\nОчищение памяти \n");
                free_memory(book);
                book = NULL;
            break;    
            case 6:
                printf("Вы вышли из программы! \n");
                return 0;     
            break; 
        }
    }while(n != 6);

}
