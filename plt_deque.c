
#include <stdlib.h>
#include "plt_deque.h"

#define SUCCESS (0)
#define FAILURE (1)

static deque_node_t *deque_node(deque_t *deque, unsigned int index)
{
    unsigned int i = 0;
    deque_node_t *node = NULL;

    if(deque->last_node)
    {
        if(deque->last_index <= index)
        {
            for(i = deque->last_index, node = deque->last_node; i < index; ++i, node = node->next);
        }
        else
        {
            for(i = deque->last_index, node = deque->last_node; i > index; --i, node = node->prev);
        }
    }
    else
    {
        if(index <= deque->count / 2)
        {
            for(i = 0, node = deque->head; i < index; ++i, node = node->next);
        }
        else
        {
            for(i = deque->count - 1, node = deque->tail; i > index; --i, node = node->prev);
        }
    }

    return node;
}


deque_t *deque_create(void)
{
    deque_t *root = (deque_t *)malloc(sizeof(deque_t));

    if(root)
    {
        root->head = NULL;
        root->tail = NULL;
        root->last_node = NULL;
        root->last_index = 0;
        root->count = 0;
        pthread_mutex_init(&root->mutex, NULL);
    }

    return root;
}

int deque_insert(deque_t *deque, unsigned int pos, void *data)
{
    int ret = FAILURE;
    deque_node_t *node_new = NULL, *node_old = NULL;

    if(!deque)
    {
        return ret;
    }

    node_new = (deque_node_t *)malloc(sizeof(deque_node_t));
    if(node_new)
    {
        node_new->prev = NULL;
        node_new->next = NULL;
        node_new->data = data; 

        pthread_mutex_lock(&deque->mutex);
        pos = pos > deque->count ? deque->count : pos;
        if(0 == deque->count)
        {
            deque->head = node_new;
            deque->tail = node_new;
        }
        else
        {
            if(0 == pos)
            {
                node_new->next = deque->head;
                deque->head->prev = node_new;
                deque->head = node_new;
            }
            else if(deque->count == pos)
            {
                node_new->prev = deque->tail;
                deque->tail->next = node_new;
                deque->tail = node_new;
            }
            else
            {
                node_old = deque_node(deque, pos);
                node_new->prev = node_old->prev;
                node_new->next = node_old;
                node_old->prev->next = node_new;
                node_old->prev = node_new;
            }
        }
        deque->last_node = node_new;
        deque->last_index = pos;
        deque->count += 1;
        pthread_mutex_unlock(&deque->mutex);
        ret = SUCCESS;
    }
    else
    {
        ret = FAILURE;
    }

    return ret;
}


int deque_push_front(deque_t *deque, void *data)
{
    return deque_insert(deque, 0, data);
}

void *deque_pop_front(deque_t *deque)
{
    void *data = NULL;
    if(deque && deque->count)
    {
        pthread_mutex_lock(&deque->mutex);
        if(deque->head == deque->tail)
        {
            deque->last_node = NULL;
            deque->last_index = 0;
            data = deque->head->data;
            free(deque->head);
            deque->head = NULL;
            deque->tail = NULL;
        }
        else
        {
            deque->head = deque->head->next;
            deque->last_node = deque->head;
            deque->last_index = 0;
            data = deque->head->prev->data;
            free(deque->head->prev);
            deque->head->prev = NULL;
        }
        deque->count -= 1;
        pthread_mutex_unlock(&deque->mutex);
    }

    return data;
}

int deque_push_back(deque_t *deque, void *data)
{
    return deque_insert(deque,  deque->count, data);
}

void *deque_pop_back(deque_t *deque)
{
    void *data = NULL;

    if(deque && deque->count)
    {
        pthread_mutex_lock(&deque->mutex);
        if(deque->head == deque->tail)
        {
            deque->last_node = NULL;
            deque->last_index = 0;
            data = deque->head->data;
            free(deque->head);
            deque->head = NULL;
            deque->tail = NULL;
        }
        else
        {
            deque->tail = deque->tail->prev;
            deque->last_node = deque->tail;
            deque->last_index = deque->count - 2;
            data = deque->tail->next->data;
            free(deque->tail->next);
            deque->tail->next = NULL;
        }
        deque->count -= 1;
        pthread_mutex_unlock(&deque->mutex);
    }

    return data;
}

void *deque_get_front(deque_t *deque)
{
    return deque_search(deque, 0);
}

void *deque_get_back(deque_t *deque)
{
    return deque_search(deque, deque->count-1);
}

int deque_remove(deque_t *deque, unsigned int pos)
{
    int ret = FAILURE;
    deque_node_t *node = NULL;

    if(deque && pos < deque->count)
    {
        pthread_mutex_lock(&deque->mutex);
        pos = pos > deque->count ? deque->count : pos;
        if(deque->head == deque->tail)
        {
            deque->last_node = NULL;
            deque->last_index = 0;
            free(deque->head);
            deque->head = NULL;
            deque->tail = NULL;
        }
        else
        {
            if(0 == pos)
            {
                deque->head = deque->head->next;
                deque->last_node = deque->head;
                deque->last_index = 0;
                free(deque->head->prev);
                deque->head->prev = NULL;
            }
            else if(deque->count - 1 == pos)
            {
                deque->tail = deque->tail->prev;
                deque->last_node = deque->tail;
                deque->last_index = deque->count - 2;
                free(deque->tail->next);
                deque->tail->next = NULL;
            }
            else
            {
                node = deque_node(deque, pos);
                node->prev->next = node->next;
                node->next->prev = node->prev;
                deque->last_node = node->next;
                deque->last_index = pos;
                free(node);
            }
        }
        deque->count -= 1;
        pthread_mutex_unlock(&deque->mutex);
        ret = SUCCESS;
    }
    else
    {
        ret = FAILURE;
    }

    return ret;
}


int deque_swap(deque_t *deque, unsigned int pos_new, unsigned int pos_old)
{
    int ret = FAILURE;
    deque_node_t *node_new = NULL, * node_old = NULL;
    void *data = NULL;

    if(deque && pos_new < deque->count && pos_old < deque->count)
    {
        pthread_mutex_lock(&deque->mutex);
        node_new = deque_node(deque, pos_new);
        node_old = deque_node(deque, pos_old);
        data = node_new->data;
        node_new->data = node_old->data;
        node_old->data = data;
        deque->last_node = node_new;
        deque->last_index = pos_new;
        pthread_mutex_unlock(&deque->mutex);
        ret = SUCCESS;
    }
    else
    {
        ret = FAILURE;
    }

    return ret;
}

void *deque_search(deque_t *deque, unsigned int pos)
{
    deque_node_t *node = NULL;
    void *data = NULL;

    if(deque && pos < deque->count)
    {
        pthread_mutex_lock(&deque->mutex);
        node = deque_node(deque, pos);
        if(node)
        {
            deque->last_node = node;
            deque->last_index = pos;
            data = node->data;
        }
        pthread_mutex_unlock(&deque->mutex);
    }

    return data;
}

unsigned int deque_count(deque_t *deque)
{
    return deque ? deque->count : 0;
}

int deque_destory(deque_t *deque)
{
    int ret = 0;
    deque_node_t *node = NULL;

    if(deque)
    {
        pthread_mutex_lock(&deque->mutex);
        while(deque->head)
        {
            node = deque->head;
            deque->head = deque->head->next;
            free(node);
        }
        pthread_mutex_unlock(&deque->mutex);
        pthread_mutex_destroy(&deque->mutex);
        free(deque);
        ret = SUCCESS;
    }
    else
    {
        ret = FAILURE;
    }

    return ret;
}