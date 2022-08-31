
#ifndef PLT_DEQUE_H
#define PLT_DEQUE_H

#include <pthread.h>

typedef struct deque_node_t
{
    struct deque_node_t *prev;
    struct deque_node_t *next;
    void *data;
} deque_node_t;

typedef struct deque_t
{
    deque_node_t *head;
    deque_node_t *tail;
    deque_node_t *last_node;
    unsigned int last_index;
    unsigned int count;
    pthread_mutex_t mutex;
} deque_t;

deque_t *deque_create(void);

int deque_insert(deque_t *deque, unsigned int pos, void *data);

int deque_push_front(deque_t *deque, void *data);

void *deque_pop_front(deque_t *deque);

int deque_push_back(deque_t *deque, void *data);

void *deque_pop_back(deque_t *deque);

void *deque_get_front(deque_t *deque);

void *deque_get_back(deque_t *deque);

int deque_remove(deque_t *deque, unsigned int pos);

int deque_swap(deque_t *deque, unsigned int pos_new, unsigned int pos_old);

void *deque_search(deque_t *deque, unsigned int pos);

unsigned int deque_count(deque_t *deque);

int deque_destory(deque_t *deque);

#endif