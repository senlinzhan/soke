#include "workq.hpp"

#include <errno.h>

int VALID = 100;

int workq_init(workq *q, int threads, void (*engine)(void *))
{
    int status = pthread_attr_init(&q->attr);
    if (status != 0)
    {
        return status;
    }
    status = pthread_attr_setdetachstate(&q->attr, PTHREAD_CREATE_DETACHED);
    if (status != 0)
    {
        pthread_attr_destroy(&q->attr);
        return status;
    }
    status = pthread_mutex_init(&q->mutex, nullptr);
    if (status != 0)
    {
        pthread_attr_destroy(&q->attr);
        return status;
    }
    status = pthread_cond_init(&q->cond, nullptr);
    if (status != 0)
    {
        pthread_mutex_destroy(&q->mutex);
        pthread_attr_destroy(&q->attr);
        return status;
    }
    q->quit = false;
    q->first = q->last = nullptr;
    q->parallelism = threads;
    q->counter = 0;
    q->idle = 0;
    q->engine = engine;
    q->valid = VALID; // FIXME

    return 0;
}


int workq_destroy(workq *q)
{
    int status, status1, status2;
    if (q->valid != VALID)
    {
        return EINVAL;
    }

    status = pthread_mutex_lock(&q->mutex);
    if (status != 0)
    {
        return status;
    }
    q->valid = 0;

    if (q->counter > 0)
    {
        q->quit = true;

        if (q->idle > 0)
        {
            status = pthread_cond_broadcast(&q->cond);
            if (status != 0)
            {
                pthread_mutex_unlock(&q->mutex);
                return status;
            }
        }

        while (q->counter > 0)
        {
            status = pthread_cond_wait(&q->cond, &q->mutex);
            if (status != 0)
            {
                pthread_mutex_unlock(&q->mutex);
                return status;
            }
        }
    }

    status = pthread_mutex_unlock(&q->mutex);
    if (status != 0)
    {
        return status;
    }
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    pthread_attr_destroy(&q->attr);

    return 0;
}


