# ifndef __LINKEDLIST_H__
# define __LINKEDLIST_H__ 

# include "./stdtype.h"


typedef struct Node Node;
struct Node{
    Node *prev;
    Node *next;
};

typedef struct{
    Node head;
    Node tail;

} LinkedList;

// 回调函数
typedef bool (*list_condition)(Node *node ,int arg); 

void list_init(LinkedList *list);
void insert_before(Node *before , Node *node);
void list_remove(Node *node);
void list_push(LinkedList *list, Node *node);
void list_append(LinkedList *list, Node *node);
Node *list_pop(LinkedList *list);
Node *list_find(LinkedList *list, Node *node);
Node *list_match(LinkedList *list , list_condition con , int arg);
uint32_t list_len(LinkedList *list);
bool list_empty(LinkedList *list);



# endif