/* Copyright [2020] IGNAT Andrei-Horia && PITUR Sebastian 314CA */

#include "LinkedList.h"

void init_list(struct LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void add_last(struct LinkedList *list, void *new_data) {
    struct Node *new_node;
    if (!list)
        return;
    new_node = malloc(sizeof(struct Node));
    new_node->data = new_data;
    list->size++;
    if (!list->head) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
    new_node->next = NULL;
}

void add_last_improved(struct LinkedList *list, void *new_data,
    int new_data_bytes) {
    struct Node *new_node;
    if (!list)
        return;
    new_node = malloc(sizeof(struct Node));
    new_node->data = malloc(new_data_bytes);
    memcpy(new_node->data, new_data, new_data_bytes);
    list->size++;
    if (!list->head) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
    new_node->next = NULL;
}

struct Node* remove_first(struct LinkedList *list) {
    struct Node *first;
    if (!list || !list->head)
        return NULL;
    first = list->head;
    if (!first->next)
        list->tail = NULL;
    list->head = first->next;
    list->size--;
    return first;
}

int get_size(struct LinkedList *list) {
    if (list == NULL) {
        return -1;
    }
    return list->size;
}

void free_list(struct LinkedList *pp_list) {
    struct Node *currNode;
    if (pp_list == NULL) {
        return;
    }
    while (get_size(pp_list) > 0) {
        currNode = remove_first(pp_list);
        free(currNode);
    }
    free(pp_list);
    pp_list = NULL;
}

void free_list_improved(struct LinkedList *pp_list) {
    struct Node *currNode;
    if (pp_list == NULL) {
        return;
    }
    while (get_size(pp_list) > 0) {
        currNode = remove_first(pp_list);
        free(currNode->data);
        free(currNode);
    }
    free(pp_list);
    pp_list = NULL;
}
