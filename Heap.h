/* Copyright [2020] IGNAT Andrei-Horia && PITUR Sebastian 314CA */

#ifndef __HEAP_H_
#define __HEAP_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GO_UP(x) (((x) - 1) >> 1)
#define GO_LEFT(x) (((x) << 1) + 1)
#define GO_RIGHT(x) (((x) << 1) + 2)

struct Heap {
    /* heap elements */
    void **arr;
    int size, capacity;
    /* function used for sorting the keys */
    int (*cmp)(void*, void*);
};

/**
 * Alloc memory for a new heap
 * @cmp_f: pointer to a function used for sorting
 * @return: pointer to the newly created heap
 */
void init_heap(struct Heap *heap, int capacity, int (*cmp_f)(void*, void*));

/**
 * Insert a new element in a heap
 * @heap: the heap where to insert the new element
 * @team: the team to be inserted in heap
 */
void heap_insert(struct Heap *heap, void *new_data);

/**
 * Get the top element
 * @heap: the heap where to search for the top element
 * @return: the top element
 */
void* heap_top(struct Heap *heap);

/**
 * Remove the top element
 */
void heap_pop(struct Heap *heap);

/**
 * Check if the heap is empty
 * @heap: the heap to be checked
 * @return: 1 if the heap is empty else 0
 */
int is_empty_heap(struct Heap *heap);

/**
 * Free a heap
 * @heap: the heap to be freed
 */
void free_heap(struct Heap *heap);

#endif
