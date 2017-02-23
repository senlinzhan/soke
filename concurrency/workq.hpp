#ifndef WORKQ_H
#define WORKQ_H

#include <pthread.h>

typedef struct workq_elem
{
    struct workq_elem_tag *next;
    void                  *data;
} workq_elem;

typedef struct workq
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    pthread_attr_t  attr;
    workq_elem      *first, *last;
    int             valid;
    bool             quit;
    int             parallelism;
    int             counter;
    int             idle;
    void            (*engine)(void *arg);
} workq;

int workq_init(workq *q, int threads, void (*engine)(void *));
int workq_destroy(workq *q);
int workq_add(workq *q, void *data);


#endif /* WORKQ_H */
