# include "./linkedlist.h"


// 链表初始化
void list_init(LinkedList *list) {
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
}

// 插入指定节点之前
void insert_before(Node *before , Node *node){
    //intr_status old = get_intr_status();
    disable_intr();

    node->next = before;
    node->prev = before->prev;
    before->prev->next = node;
    before->prev = node;
    
    //set_intr_status(old);
}
// 删除指定节点
void list_remove(Node *node){
    //intr_status old = get_intr_status();
    //disable_intr();

    node->prev->next = node->next;
    node->next->prev = node->prev;

    //set_intr_status(old);
}
// 头插
void list_push(LinkedList *list, Node *node) {
    insert_before(list->head.next, node);
}

// 尾插
void list_append(LinkedList *list, Node *node) {
    insert_before(&list->tail, node);
}

// 头删
Node *list_pop(LinkedList *list) {
    //intr_status old = get_intr_status();
    disable_intr();

    Node *node = list->head.next;

    if (node == &list->tail) return NULL;

    list_remove(node);

    //set_intr_status(old);
    return node;
}

// 查找 线程不安全
Node *list_find(LinkedList *list, Node *node) {
    Node *p = list->head.next;

    while (p != &list->tail) {
        if (p == node) return p;
        p = p->next;
    }

    return NULL;
}

// 自定义查找 线程不安全
Node *list_match(LinkedList *list , list_condition con , int arg){
    Node *p = list->head.next;
    while (p != &list->tail) {
        if (con(p , arg)) return p;
        p = p->next;
    }
    return NULL;
}

// 链表长度 线程不安全
uint32_t list_len(LinkedList *list) {
    uint32_t len = 0;
    Node *p = list->head.next;

    while (p != &list->tail) {
        len++;
        p = p->next;
    }

    return len;
}

// 判空 线程不安全
bool list_empty(LinkedList *list) {
    return list->head.next == &list->tail;
}


