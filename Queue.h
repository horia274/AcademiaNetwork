/* Copyright [2020] IGNAT Andrei-Horia && PITUR Sebastian 314CA */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdio.h>
#include <stdlib.h>

#include "LinkedList.h"

struct Queue {
    struct LinkedList *list;
};

void init_q(struct Queue *q);

int get_size_q(struct Queue *q);

int is_empty_q(struct Queue *q);

void* front(struct Queue *q);

void dequeue(struct Queue *q);

void dequeue_improved(struct Queue *q);

void enqueue(struct Queue *q, void *new_data);

void enqueue_improved(struct Queue *q, void *new_data, int new_data_bytes);

void purge_q(struct Queue *q);

void purge_q_improved(struct Queue *q);

#endif /* __QUEUE_H__ */
