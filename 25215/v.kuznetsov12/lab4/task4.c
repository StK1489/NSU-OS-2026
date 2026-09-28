#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char *str;
    struct node *next;
};

int main()
{
    char buffer[BUFSIZ];
    struct node *head = NULL;
    struct node *tail = NULL;
    struct node *p;
    int line_start = 1;
    printf("Ввод (точка в начале строки - конец):\n");
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (line_start && buffer[0] == '.') {
            break;
        }
        size_t len = strlen(buffer);
        line_start = (len > 0 && buffer[len - 1] == '\n');
        char *new_str = malloc(len + 1);
        if (new_str == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            break;
        }
        strcpy(new_str, buffer);
        struct node *new_node = malloc(sizeof(struct node));
        if (new_node == NULL) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            free(new_str);
            break;
        }
        new_node->str = new_str;
        new_node->next = NULL;
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }
    printf("\nВсе строки:\n");
    for (p = head; p != NULL; p = p->next) {
        printf("%s", p->str);
    }
    p = head;
    while (p != NULL) {
        struct node *next = p->next;
        free(p->str);
        free(p);
        p = next;
    }
    return 0;
}