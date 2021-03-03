/* Copyright [2020] IGNAT Andrei-Horia && PITUR Sebastian 314CA */

#include "publications.h"

struct paper {
    int64_t id;
    char *title;
    int year;
    int num_cits;
    int num_refs;
    int64_t *references;
    int num_fields;
    char **fields;
    int visit;
    int dist;
};

struct author {
    char *name;
    struct LinkedList *id_papers;
    struct LinkedList *venues;
    struct LinkedList *institutions;
    int visit;
    int dist;
};

struct publications_data {
    /* TODO: add whichever data you need here */
    struct Hashtable *ht_papers;
    struct Hashtable *ht_citations;
    struct Hashtable *ht_venues;
    struct Hashtable *ht_authors;
    struct Hashtable *ht_collabarations;
    struct Hashtable *ht_fields;
    struct Hashtable *ht_institutions;
    int *years;
};

void free_paper_ht(void *info) {
    int i;

    free(((struct paper*)info)->title);
    free(((struct paper*)info)->references);
    for (i = 0; i < ((struct paper*)info)->num_fields; i++)
        free(((struct paper*)info)->fields[i]);
    free(((struct paper*)info)->fields);
    free(((struct paper*)info));
}

void free_list_ht(void *info) {
    free_list_improved((struct LinkedList*)info);
}

void free_author_ht(void *info) {
    free(((struct author*)info)->name);
    free_list_improved(((struct author*)info)->id_papers);
    free_list_improved(((struct author*)info)->venues);
    free_list_improved(((struct author*)info)->institutions);
    free((struct author*)info);
}

void reset_visit_papers(PublData* data, struct Queue *v) {
    int index;
    struct Node *curr;
    struct paper *info;

    while (!is_empty_q(v)) {
        index = *(int*)front(v);
        curr = data->ht_papers->buckets[index].head;
        while (curr) {
            info = (struct paper*)((struct info*)curr->data)->value;
            info->visit = 0;
            info->dist = 0;
            curr = curr->next;
        }
        dequeue_improved(v);
    }
}

void reset_visit_authors(PublData* data, struct Queue *v) {
    int index;
    struct Node *curr;
    struct author *info;

    while (!is_empty_q(v)) {
        index = *(int*)front(v);
        curr = data->ht_authors->buckets[index].head;
        while (curr) {
            info = (struct author*)((struct info*)curr->data)->value;
            info->visit = 0;
            info->dist = 0;
            curr = curr->next;
        }
        dequeue_improved(v);
    }
}

int compare_papers(void *key1, void *key2) {
    struct paper *paper1 = (struct paper*)key1, *paper2 = (struct paper*)key2;

    if (paper1->num_cits != paper2->num_cits)
        return paper2->num_cits - paper1->num_cits;
    else if (paper1->year != paper2->year)
        return paper2->year - paper1->year;
    else
        return paper1->id - paper2->id;
}

int compare_reading_papers(void *key1, void *key2) {
    struct paper *paper1 = (struct paper*)key1, *paper2 = (struct paper*)key2;

    if (paper1->year != paper2->year)
        return paper1->year - paper2->year;
    else
        return paper1->id - paper2->id;
}

PublData* init_publ_data(void) {
    /* TODO: implement init_publ_data */
    PublData *data;
    data = malloc(sizeof(PublData));

    data->ht_papers = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_papers, HMAX, hash_function_int,
        compare_function_ints);

    data->ht_citations = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_citations, HMAX, hash_function_int,
        compare_function_ints);

    data->ht_venues = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_venues, HMAX, hash_function_string,
        compare_function_strings);

    data->ht_authors = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_authors, HMAX, hash_function_int,
        compare_function_ints);

    data->ht_collabarations = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_collabarations, HMAX, hash_function_int,
        compare_function_ints);

    data->ht_fields = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_fields, HMAX, hash_function_string,
        compare_function_strings);

    data->ht_institutions = malloc(sizeof(struct Hashtable));
    init_ht(data->ht_institutions, HMAX, hash_function_string,
        compare_function_strings);

    data->years = calloc(MAX_YEAR, sizeof(int));
    return data;
}

void destroy_publ_data(PublData* data) {
    /* TODO: implement destroy_publ_data */
    free_ht(data->ht_papers, free_paper_ht);
    free_ht(data->ht_citations, free_list_ht);
    free_ht(data->ht_venues, free_list_ht);
    free_ht(data->ht_authors, free_author_ht);
    free_ht(data->ht_collabarations, free_list_ht);
    free_ht(data->ht_fields, free_list_ht);
    free_ht(data->ht_institutions, free_list_ht);
    free(data->years);
    free(data);
}

void add_paper(PublData* data, const char* title, const char* venue,
    const int year, const char** author_names, const int64_t* author_ids,
    const char** institutions, const int num_authors, const char** fields,
    const int num_fields, const int64_t id, const int64_t* references,
    const int num_refs) {
    /* TODO: implement add_paper */
    int i, j;
    struct paper *info;
    struct author *author;
    int64_t key, id_paper = id, id_author, id_author_insert;
    struct LinkedList *list, *list_ids, *list_authors;
    char *venue_key, *institution, *field;

    key = id;
    info = malloc(sizeof(struct paper));
    info->id = id;
    info->title = malloc((strlen(title) + 1) * sizeof(char));
    memcpy(info->title, title, strlen(title) + 1);
    info->year = year;
    info->num_cits = 0;
    info->num_refs = num_refs;
    info->references = malloc(num_refs * sizeof(int64_t));
    info->num_fields = num_fields;
    info->fields = malloc(num_fields * sizeof(char*));
    for (i = 0; i < num_fields; i++) {
        info->fields[i] = malloc((strlen(fields[i]) + 1) * sizeof(char));
        memcpy(info->fields[i], fields[i], strlen(fields[i]) + 1);
    }
    info->visit = 0;
    info->dist = 0;

    for (i = 0; i < num_refs; i++) {
        info->references[i] = references[i];
        list_ids = get(data->ht_citations, &info->references[i]);
        if (list_ids) {
            add_last_improved(list_ids, &id_paper, sizeof(int64_t));
        } else {
            list_ids = malloc(sizeof(struct LinkedList));
            init_list(list_ids);
            add_last_improved(list_ids, &id_paper, sizeof(int64_t));
            put(data->ht_citations, &info->references[i],
                sizeof(int64_t), list_ids);
        }
    }

    put(data->ht_papers, &key, sizeof(int64_t), info);

    venue_key = strdup(venue);
    list = get(data->ht_venues, venue_key);
    if (list) {
        add_last_improved(list, &id_paper, sizeof(int64_t));
    } else {
        list = malloc(sizeof(struct LinkedList));
        init_list(list);
        add_last_improved(list, &id_paper, sizeof(int64_t));
        put(data->ht_venues, venue_key, strlen(venue_key) + 1, list);
    }

    for (i = 0; i < num_authors; i++) {
        id_author = author_ids[i];
        institution = strdup(institutions[i]);
        author = get(data->ht_authors, &id_author);
        if (author) {
            add_last_improved(author->id_papers, &id_paper, sizeof(int64_t));
            add_last_improved(author->venues, venue_key, strlen(venue_key) + 1);
            add_last_improved(author->institutions, institution,
                (strlen(institution) + 1) * sizeof(char));
        } else {
            author = malloc(sizeof(struct author));
            author->name = malloc((strlen(author_names[i]) + 1) * sizeof(char));
            memcpy(author->name, author_names[i], strlen(author_names[i]) + 1);
            author->id_papers = malloc(sizeof(struct LinkedList));
            init_list(author->id_papers);
            add_last_improved(author->id_papers, &id_paper, sizeof(int64_t));
            author->venues = malloc(sizeof(struct LinkedList));
            init_list(author->venues);
            add_last_improved(author->venues, venue_key, strlen(venue) + 1);
            author->institutions = malloc(sizeof(struct LinkedList));
            init_list(author->institutions);
            add_last_improved(author->institutions, institution,
                (strlen(institution) + 1) * sizeof(char));
            author->visit = 0;
            author->dist = 0;
            put(data->ht_authors, &id_author, sizeof(int64_t), author);
        }

        list_authors = get(data->ht_collabarations, &id_author);
        if (list_authors) {
            for (j = 0; j < num_authors; j++) {
                id_author_insert = author_ids[j];
                add_last_improved(list_authors, &id_author_insert,
                    sizeof(int64_t));
            }
        } else {
            list_authors = malloc(sizeof(struct LinkedList));
            init_list(list_authors);
            for (j = 0; j < num_authors; j++) {
                id_author_insert = author_ids[j];
                add_last_improved(list_authors, &id_author_insert,
                    sizeof(int64_t));
            }
            put(data->ht_collabarations, &id_author,
                sizeof(int64_t), list_authors);
        }

        list_authors = get(data->ht_institutions, institution);
        if (list_authors) {
            id_author_insert = author_ids[i];
            add_last_improved(list_authors, &id_author_insert, sizeof(int64_t));
        } else {
            list_authors = malloc(sizeof(struct LinkedList));
            init_list(list_authors);
            id_author_insert = author_ids[i];
            add_last_improved(list_authors, &id_author_insert, sizeof(int64_t));
            put(data->ht_institutions, institution,
                strlen(institution) + 1, list_authors);
        }

        free(institution);
    }
    free(venue_key);

    for (i = 0; i < num_fields; i++) {
        field = strdup(fields[i]);
        list_ids = get(data->ht_fields, field);
        if (list_ids) {
            add_last_improved(list_ids, &id_paper, sizeof(int64_t));
        } else {
            list_ids = malloc(sizeof(struct LinkedList));
            init_list(list_ids);
            add_last_improved(list_ids, &id_paper, sizeof(int64_t));
            put(data->ht_fields, field, strlen(field) + 1, list_ids);
        }
        free(field);
    }

    data->years[year]++;
}

char* get_oldest_influence(PublData* data, const int64_t id_paper) {
    /* TODO: implement get_oldest_influence */
    struct Queue *q, *v;
    int64_t id = id_paper, min_id;
    struct paper *info, *info_top;
    struct LinkedList *list;
    int min_year = MAX_YEAR, index, i, max_num_cits = 0, cits;
    char *title = "None";

    info = get(data->ht_papers, &id);
    if (!info || !info->num_refs)
        return title;

    q = malloc(sizeof(struct Queue));
    init_q(q);
    info->visit = 1;
    enqueue(q, info);

    v = malloc(sizeof(struct Queue));
    index = data->ht_papers->hash_function(&id) % HMAX;
    init_q(v);
    enqueue_improved(v, &index, sizeof(int));

    while (!is_empty_q(q)) {
        info_top = (struct paper*)front(q);
        dequeue(q);
        for (i = 0; i < info_top->num_refs; i++) {
            id = info_top->references[i];
            info = get(data->ht_papers, &id);
            if (info && info->visit == 0) {
                info->visit = 1;
                index = data->ht_papers->hash_function(&id) % HMAX;
                enqueue(q, info);
                enqueue_improved(v, &index, sizeof(int));
                list = get(data->ht_citations, &id);
                cits = get_size(list);

                if (min_year > info->year) {
                    title = info->title;
                    min_year = info->year;
                    max_num_cits = cits;
                    min_id = id;
                } else if (min_year == info->year) {
                    if (max_num_cits < cits) {
                        title = info->title;
                        max_num_cits = cits;
                        min_id = id;
                    } else if (max_num_cits == cits) {
                        if (min_id > id) {
                            title = info->title;
                            min_id = id;
                        }
                    }
                }
            }
        }
    }

    reset_visit_papers(data, v);
    purge_q_improved(v);
    purge_q(q);

    return title;
}

float get_venue_impact_factor(PublData* data, const char* venue) {
    /* TODO: implement get_venue_impact_factor */
    struct LinkedList *list;
    struct Node *curr;
    int num, sum = 0;
    char *venue_key = strdup(venue);
    int64_t id;
    float factor;

    list = get(data->ht_venues, venue_key);
    num = get_size(list);
    curr = list->head;
    while (curr) {
        id = *(int*)curr->data;
        list = get(data->ht_citations, &id);
        if (list)
            sum += get_size(list);
        curr = curr->next;
    }
    free(venue_key);
    factor = (float) sum / num;

    return factor;
}

int get_number_of_influenced_papers(PublData* data, const int64_t id_paper,
    const int distance) {
    /* TODO: implement get_number_of_influenced_papers */
    int64_t id = id_paper, id_top;
    struct LinkedList *list;
    struct Queue *q, *v;
    struct Node *curr;
    struct paper *info, *info_top;
    int num_infl = 0, index;

    info = get(data->ht_papers, &id);
    if (!info)
        return num_infl;

    q = malloc(sizeof(struct Queue));
    init_q(q);
    info->visit = 1;
    enqueue_improved(q, &id, sizeof(int64_t));

    v = malloc(sizeof(struct Queue));
    index = data->ht_papers->hash_function(&id) % data->ht_papers->hmax;
    init_q(v);
    enqueue_improved(v, &index, sizeof(int));

    while (!is_empty_q(q)) {
        id_top = *(int64_t*)front(q);
        info_top = get(data->ht_papers, &id_top);
        dequeue_improved(q);

        if (info_top->dist > distance)
            break;

        list = get(data->ht_citations, &id_top);
        if (list) {
            curr = list->head;
            while (curr) {
                id = *(int64_t*)curr->data;
                info = get(data->ht_papers, &id);
                if (info->visit == 0) {
                    info->visit = 1;
                    info->dist = info_top->dist + 1;
                    if (info->dist <= distance)
                        num_infl++;
                    index = data->ht_papers->hash_function(&id) % HMAX;
                    enqueue_improved(q, &id, sizeof(int64_t));
                    enqueue_improved(v, &index, sizeof(int));
                }
                curr = curr->next;
            }
        }
    }

    reset_visit_papers(data, v);
    purge_q_improved(v);
    purge_q_improved(q);

    return num_infl;
}

int get_erdos_distance(PublData* data, const int64_t id1, const int64_t id2) {
    /* TODO: implement get_erdos_distance */
    int64_t id = id1, id_top;
    struct author *author, *author_top;
    struct Queue *q, *v;
    struct LinkedList *list;
    struct Node *curr;
    int index, min_dist = MAX_DIST, found = 0;

    author = get(data->ht_authors, &id);
    if (!author)
        return -1;

    q = malloc(sizeof(struct Queue));
    init_q(q);
    author->visit = 1;
    enqueue_improved(q, &id, sizeof(int64_t));

    v = malloc(sizeof(struct Queue));
    index = data->ht_authors->hash_function(&id) % HMAX;
    init_q(v);
    enqueue_improved(v, &index, sizeof(int));

    while (!found && !is_empty_q(q)) {
        id_top = *(int64_t*)front(q);
        author_top = get(data->ht_authors, &id_top);
        dequeue_improved(q);

        list = get(data->ht_collabarations, &id_top);
        if (list) {
            curr = list->head;
            while (curr) {
                id = *(int64_t*)curr->data;
                author = get(data->ht_authors, &id);

                if (author->visit == 0) {
                    author->visit = 1;
                    author->dist = author_top->dist + 1;
                    index = data->ht_authors->hash_function(&id) % HMAX;
                    enqueue_improved(q, &id, sizeof(int64_t));
                    enqueue_improved(v, &index, sizeof(int));
                    if (id == id2) {
                        found = 1;
                        min_dist = author->dist;
                        break;
                    }
                }
                curr = curr->next;
            }
        }
    }

    reset_visit_authors(data, v);
    purge_q_improved(v);
    purge_q_improved(q);

    if (min_dist != MAX_DIST)
        return min_dist;
    return -1;
}

char** get_most_cited_papers_by_field(PublData* data, const char* field,
    int* num_papers) {
    /* TODO: implement get_most_cited_papers_by_field */
    char **papers = NULL, *copy_field = strdup(field);
    int64_t id;
    int count;
    struct paper *paper;
    struct LinkedList *list_ids, *list_cits;
    struct Node *curr_id;
    struct Heap *heap;

    list_ids = get(data->ht_fields, copy_field);
    if (!list_ids)
        return papers;

    if (get_size(list_ids) < *num_papers)
        *num_papers = get_size(list_ids);

    heap = malloc(sizeof(struct Heap));
    init_heap(heap, CAPACITY, compare_papers);

    curr_id = list_ids->head;
    while (curr_id) {
        id = *(int64_t*)curr_id->data;
        paper = get(data->ht_papers, &id);
        list_cits = get(data->ht_citations, &id);
        if (list_cits)
            paper->num_cits = get_size(list_cits);
        heap_insert(heap, paper);
        curr_id = curr_id->next;
    }

    papers = malloc(*num_papers * sizeof(char*));
    count = 0;
    while (count < *num_papers) {
        paper = (struct paper*)heap_top(heap);
        papers[count] = strdup(paper->title);
        count++;
        heap_pop(heap);
    }

    free(copy_field);
    free_heap(heap);

    return papers;
}

int get_number_of_papers_between_dates(PublData* data, const int early_date,
    const int late_date) {
    /* TODO: implement get_number_of_papers_between_dates */
    int i, num_papers = 0;

    for (i = early_date; i <= late_date; i++)
        num_papers += data->years[i];

    return num_papers;
}

int get_number_of_authors_with_field(PublData* data, const char* institution,
    const char* field) {
    /* TODO: implement get_number_of_authors_with_field */
    int num_authors = 0, index, found, i;
    char *inst = strdup(institution);
    int64_t id_author, id_paper;
    struct author* author;
    struct paper *paper;
    struct LinkedList *list_authors, *list_papers, *list_institutions;
    struct Node *curr_author, *curr_paper, *curr_institution;
    struct Queue *v;

    v = malloc(sizeof(struct Queue));
    init_q(v);

    list_authors = get(data->ht_institutions, inst);
    if (list_authors) {
        curr_author = list_authors->head;
        while (curr_author) {
            id_author = *(int64_t*)curr_author->data;
            author = get(data->ht_authors, &id_author);
            if (author && !author->visit) {
                author->visit = 1;
                index = data->ht_authors->hash_function(&id_author) % HMAX;
                enqueue_improved(v, &index, sizeof(int));
                list_papers = author->id_papers;
                list_institutions = author->institutions;
                if (list_papers) {
                    curr_paper = list_papers->head;
                    curr_institution = list_institutions->head;
                    found = 0;

                    while (!found && curr_paper) {
                        if (curr_institution &&
                        !strcmp((char*)curr_institution->data, inst)) {
                            id_paper = *(int64_t*)curr_paper->data;
                            paper = get(data->ht_papers, &id_paper);
                            if (paper) {
                                for (i = 0; i < paper->num_fields; i++) {
                                    if (!strcmp(paper->fields[i], field)) {
                                        num_authors++;
                                        found = 1;
                                        break;
                                    }
                                }
                            }
                        }
                        curr_paper = curr_paper->next;
                        curr_institution = curr_institution->next;
                    }
                }
            }
            curr_author = curr_author->next;
        }
    }

    reset_visit_authors(data, v);
    free(inst);
    purge_q_improved(v);

    return num_authors;
}

int* get_histogram_of_citations(PublData* data, const int64_t id_author,
    int* num_years) {
    /* TODO: implement get_histogram_of_citations */
    int *histogram, year;
    int64_t id = id_author, id_paper;
    struct author *author;
    struct LinkedList *id_papers, *list_citations;
    struct Node *curr_paper;
    struct paper *paper;

    *num_years = 0;
    author = get(data->ht_authors, &id);
    id_papers = author->id_papers;

    if (id_papers) {
        curr_paper = id_papers->head;
        while (curr_paper) {
            id_paper = *(int64_t*)curr_paper->data;
            paper = get(data->ht_papers, &id_paper);
            year = paper->year;
            if (CURRENT_YEAR - *num_years + 1 > year) {
                *num_years = CURRENT_YEAR - year + 1;
            }
            curr_paper = curr_paper->next;
        }
    }

    histogram = calloc(*num_years, sizeof(int));
    if (*num_years) {
        curr_paper = id_papers->head;
        while (curr_paper) {
            id_paper = *(int64_t*)curr_paper->data;
            paper = get(data->ht_papers, &id_paper);
            year = paper->year;
            list_citations = get(data->ht_citations, &id_paper);
            if (list_citations) {
                histogram[CURRENT_YEAR - year] += get_size(list_citations);
            }
            curr_paper = curr_paper->next;
        }
    }

    return histogram;
}

char** get_reading_order(PublData* data, const int64_t id_paper,
    const int distance, int* num_papers) {
    /* TODO: implement get_reading_order */
    int64_t id = id_paper, id_top, id_ref;
    int index, i, j, count, leaf, ok;
    char **papers = NULL;
    struct Queue *q, *v;
    struct Heap *heap;
    struct Node *curr_id;
    struct LinkedList *list_cits, *leaves;
    struct Node *curr_paper, *curr_paper_compared, *curr_id_comp;
    struct paper *paper, *paper_top, *paper_ref, *paper_compared;

    *num_papers = 0;
    paper = get(data->ht_papers, &id);
    if (!paper)
        return papers;

    heap = malloc(sizeof(struct Heap));
    init_heap(heap, CAPACITY, compare_reading_papers);

    leaves = malloc(sizeof(struct LinkedList));
    init_list(leaves);

    q = malloc(sizeof(struct Queue));
    init_q(q);
    enqueue_improved(q, &id, sizeof(int64_t));

    v = malloc(sizeof(struct Queue));
    init_q(v);

    while (!is_empty_q(q)) {
        id_top = *(int64_t*)front(q);
        paper_top = get(data->ht_papers, &id_top);
        dequeue_improved(q);

        if (paper_top->dist > distance)
            break;

        for (i = 0; i < paper_top->num_refs; i++) {
            id = paper_top->references[i];
            paper = get(data->ht_papers, &id);
            if (paper && paper->visit == 0) {
                paper->visit = 1;
                paper->dist = paper_top->dist + 1;

                if (paper->dist <= distance) {
                    (*num_papers)++;

                    leaf = 1;
                    for (j = 0; j < paper->num_refs; j++) {
                        id_ref = paper->references[j];
                        paper_ref = get(data->ht_papers, &id_ref);
                        if (paper_ref) {
                            leaf = 0;
                            break;
                        }
                    }
                    if (leaf || paper->dist == distance) {
                        add_last(leaves, paper);
                    }
                }

                index = data->ht_papers->hash_function(&id) % HMAX;
                enqueue_improved(q, &id, sizeof(int64_t));
                enqueue_improved(v, &index, sizeof(int));
            }
        }
    }

    if (leaves->head) {
        curr_paper = leaves->head;
        while (curr_paper) {
            paper = (struct paper*)curr_paper->data;
            leaf = 1;
            curr_paper_compared = leaves->head;
            while (curr_paper_compared) {
                paper_compared = (struct paper*)curr_paper_compared->data;
                for (i = 0; i < paper->num_refs; i++) {
                    if (paper->references[i] == paper_compared->id) {
                        leaf = 0;
                        break;
                    }
                }
                if (!leaf) {
                    break;
                }
                curr_paper_compared = curr_paper_compared->next;
            }
            if (leaf) {
                heap_insert(heap, paper);
            }
            curr_paper = curr_paper->next;
        }
    }

    papers = malloc(*num_papers * sizeof(char*));
    count = 0;
    while (count < *num_papers) {
        paper = (struct paper*)heap_top(heap);
        papers[count] = strdup(paper->title);
        heap_pop(heap);
        list_cits = get(data->ht_citations, &paper->id);
        if (list_cits) {
            curr_id = list_cits->head;
            while (curr_id) {
                id = *(int64_t*)curr_id->data;
                paper = get(data->ht_papers, &id);
                if (paper->visit) {
                    ok = 1;
                    for (i = 0; i < heap->size; i++) {
                        for (j = 0; j < paper->num_refs; j++) {
                            if (((struct paper*)heap->arr[i])->id ==
                            paper->references[j]) {
                                ok = 0;
                                break;
                            }
                            if (!ok)
                                break;
                        }
                    }
                    curr_id_comp = list_cits->head;
                    while (curr_id_comp) {
                        for (i = 0; i < paper->num_refs; i++) {
                            if (paper->references[i] ==
                            *(int64_t*)curr_id_comp->data) {
                                ok = 0;
                                break;
                            }
                        }
                        if (!ok)
                            break;
                        curr_id_comp = curr_id_comp->next;
                    }
                    if (ok) {
                        paper->visit = 0;
                        heap_insert(heap, paper);
                    }
                }
                curr_id = curr_id->next;
            }
        }
        count++;
    }

    reset_visit_papers(data, v);
    purge_q_improved(v);
    purge_q_improved(q);
    free_list(leaves);
    free_heap(heap);

    return papers;
}

char* find_best_coordinator(PublData* data, const int64_t id_author) {
    /* TODO: implement find_best_coordinator */
    int64_t id = id_author, id_top, min_id, id_paper;
    char *name, *venue;
    struct author *author, *author_top;
    struct Queue *q, *v;
    struct LinkedList *list, *list_papers, *list_venues, *list_cits;
    struct Node *curr_id, *curr_venue;
    struct Node *curr;
    int index, min_erd_dist = MAX_DIST, erd_dist, num_cits;
    float score, max_score = -1, impact_factor;

    author = get(data->ht_authors, &id);
    if (!author)
        return "None";

    q = malloc(sizeof(struct Queue));
    init_q(q);
    author->visit = 1;
    enqueue_improved(q, &id, sizeof(int64_t));

    v = malloc(sizeof(struct Queue));
    index = data->ht_authors->hash_function(&id) % HMAX;
    init_q(v);
    enqueue_improved(v, &index, sizeof(int));

    while (!is_empty_q(q)) {
        id_top = *(int64_t*)front(q);
        author_top = get(data->ht_authors, &id_top);
        dequeue_improved(q);

        if (author_top->dist > 5)
            break;

        list = get(data->ht_collabarations, &id_top);
        if (list) {
            curr = list->head;
            while (curr) {
                id = *(int64_t*)curr->data;
                author = get(data->ht_authors, &id);

                if (author->visit == 0) {
                    author->visit = 1;
                    author->dist = author_top->dist + 1;
                    erd_dist = author->dist;
                    index = data->ht_authors->hash_function(&id) % HMAX;
                    enqueue_improved(q, &id, sizeof(int64_t));
                    enqueue_improved(v, &index, sizeof(int));
                    if (erd_dist >= 1 && erd_dist <= 5) {
                        score = 0;
                        list_papers = author->id_papers;
                        list_venues = author->venues;
                        if (list_papers) {
                            curr_id = list_papers->head;
                            curr_venue = list_venues->head;
                            while (curr_id) {
                                id_paper = *(int64_t*)curr_id->data;
                                venue = (char*)curr_venue->data;
                                list_cits = get(data->ht_citations, &id_paper);
                                if (list_cits) {
                                    num_cits = get_size(list_cits);
                                    impact_factor =
                                    get_venue_impact_factor(data, venue);
                                    score += (num_cits * impact_factor);
                                }
                                curr_id = curr_id->next;
                                curr_venue = curr_venue->next;
                            }
                        }
                        score *= exp(-erd_dist);

                        if (score > max_score) {
                            max_score = score;
                            name = author->name;
                            min_erd_dist = erd_dist;
                            min_id = id;
                        } else if (score == max_score &&
                                min_erd_dist > erd_dist) {
                            name = author->name;
                            min_erd_dist = erd_dist;
                            min_id = id;
                        } else if (score == max_score &&
                            min_erd_dist == erd_dist && min_id > id) {
                            name = author->name;
                            min_id = id;
                        }
                    }
                }
                curr = curr->next;
            }
        }
    }

    reset_visit_authors(data, v);
    purge_q_improved(v);
    purge_q_improved(q);

    if (max_score != -1)
        return name;
    return "None";
}
