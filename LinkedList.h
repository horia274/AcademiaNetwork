/* Copyright [2020] IGNAT Andrei-Horia && PITUR Sebastian 314CA */

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    void *data;
    struct Node *next;
};

struct LinkedList {
    struct Node *head;
    struct Node *tail;
    int size;
};

void init_list(struct LinkedList *list);

void add_last(struct LinkedList *list, void *new_data);

void add_last_improved(struct LinkedList *list, void *new_data,
    int new_data_bytes);

struct Node* remove_first(struct LinkedList *list);

int get_size(struct LinkedList *list);

void free_list(struct LinkedList *list);

void free_list_improved(struct LinkedList *list);

#endif /* __LINKEDLIST_H__ */
