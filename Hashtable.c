/* Copyright [2020] IGNAT Andrei-Horia && PITUR Sebastian 314CA */

#include "Hashtable.h"

/*
 * Compare function:
 */
int compare_function_ints(void *a, void *b) {
    int64_t int_a = *((int64_t *)a);
    int64_t int_b = *((int64_t *)b);
    if (int_a == int_b) {
        return 0;
    } else if (int_a < int_b) {
        return -1;
    } else {
        return 1;
    }
}

int compare_function_strings(void *a, void *b) {
    char *str_a = (char *)a;
    char *str_b = (char *)b;
    return strcmp(str_a, str_b);
}

/*
 * Hash function:
 */
unsigned int hash_function_int(void *a) {
    /*
     * Credits: https://stackoverflow.com/a/12996028/7883884
     */
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_string(void *a) {
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char*) a;
    int64_t hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

void init_ht(struct Hashtable *ht, int hmax,
    unsigned int (*hash_function)(void*),
    int (*compare_function)(void*, void*)) {
    int i;
    ht->buckets = malloc(hmax * sizeof(struct LinkedList));
    for (i = 0; i < hmax; i++)
        init_list(&ht->buckets[i]);
    ht->size = 0;
    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;
}

void put(struct Hashtable *ht, void *key, int key_size_bytes, void *value) {
    int i;
    struct Node *curr;
    struct info *new_data;
    i = ht->hash_function(key) % ht->hmax;
    curr = ht->buckets[i].head;
    while (curr != NULL &&
        ht->compare_function(((struct info*)curr->data)->key, key) != 0) {
        curr = curr->next;
    }
    if (curr != NULL) {
        ((struct info*)curr->data)->value = value;
    } else {
        new_data = malloc(sizeof(struct info));
        new_data->key = malloc(key_size_bytes);
        memcpy(new_data->key, key, key_size_bytes);
        new_data->value = value;
        add_last(&ht->buckets[i], new_data);
        ht->size++;
    }
}

void* get(struct Hashtable *ht, void *key) {
    int i;
    struct Node *curr;
    i = ht->hash_function(key) % ht->hmax;
    curr = ht->buckets[i].head;
    while (curr != NULL &&
        ht->compare_function(((struct info*)curr->data)->key, key) != 0)
        curr = curr->next;
    if (curr != NULL)
        return ((struct info*)curr->data)->value;
    return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable folosind functia put
 * 0, altfel.
 */
int has_key(struct Hashtable *ht, void *key) {
    int i;
    struct Node *curr;
    i = ht->hash_function(key) % ht->hmax;
    curr = ht->buckets[i].head;
    while (curr != NULL &&
        ht->compare_function(((struct info*)curr->data)->key, key) != 0)
        curr = curr->next;
    if (curr != NULL)
        return 1;
    return 0;
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable, dupa care elibereaza si memoria folosita
 * pentru a stoca structura hashtable.
 */
void free_ht(struct Hashtable *ht, void (free_function)(void*)) {
    int i;
    struct Node *curr, *prev;
    for (i = 0; i < ht->hmax; i++) {
        curr = ht->buckets[i].head;
        while (curr) {
            prev = curr;
            curr = curr->next;
            free(((struct info*)prev->data)->key);
            free_function(((struct info*)prev->data)->value);
            free(prev->data);
            free(prev);
        }
    }
    free(ht->buckets);
    free(ht);
}

int get_ht_size(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }
    return ht->size;
}

int get_ht_hmax(struct Hashtable *ht) {
    if (ht == NULL) {
        return -1;
    }
    return ht->hmax;
}
