#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

typedef struct threadpool_task_t
{
    void (*function)(void *);    
    void *argument;
    struct threadpool_task_t *next;    
} threadpool_task_t;


typedef struct threadpool_t
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    threadpool_task_t *first;
    threadpool_task_t *last;

    bool quit;
    int max_threads;
    int current_threads;
    int idle_threads;
} threadpool_t;


int threadpool_create(threadpool_t *pool, int max_threads)
{
    if (max_threads <= 0)
    {
        return EINVAL;        
    }
    
    int status = pthread_mutex_init(&pool->mutex, NULL);
    if (status != 0)
    {
        return status; 
    }

    status = pthread_cond_init(&pool->cond, NULL);
    if (status != 0)
    {
        pthread_mutex_destroy(&pool->mutex);
        return status;        
    }
    
    pool->max_threads = max_threads;
    pool->current_threads = 0;
    pool->idle_threads = 0;
    pool->quit = false;

    return 0;
}

static void *threadpool_worker(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;

    struct timeval now;
    struct timespec timeout;
    
    while (true)
    {
        int status = pthread_mutex_lock(&pool->mutex);
        if (status != 0)
        {
            return NULL;
        }
        
        bool has_timeout = false;
        pool->idle_threads++;
        while (pool->first == NULL)
        {
            gettimeofday(&now, NULL);
            timeout.tv_sec = now.tv_sec + 2;
            timeout.tv_nsec = now.tv_usec * 1000;
            status = pthread_cond_timedwait(&pool->cond, &pool->mutex, &timeout);
            if (status == ETIMEDOUT)
            {
                has_timeout = true;
                break;
            }
        }
        
        pool->idle_threads--;
        threadpool_task_t *task = pool->first;
        
        if (task != NULL)
        {
            has_timeout = false;
            pool->first = task->next;
            if (pool->last == task)
            {
                pool->last = NULL;
            }
            status = pthread_mutex_unlock(&pool->mutex);
            if (status != 0)
            {
                return NULL;
            }
            task->function(task->argument);
            free(task);

            status = pthread_mutex_lock(&pool->mutex);
            if (status != 0)
            {
                return NULL;
            }
        }

        if (pool->first != NULL && has_timeout)
        {
            pool->current_threads--;
            status = pthread_mutex_unlock(&pool->mutex);
            if (status != 0)
            {
                return NULL;
            }
            break;
        }        
    }
    
    return NULL;
}

int threadpool_add(threadpool_t *pool, void (*function)(void *), void *argument)
{
    threadpool_task_t *task = (threadpool_task_t *)malloc(sizeof(threadpool_task_t));
    if (task == NULL)
    {
        return ENOMEM;
    }
    task->function = function;
    task->argument = argument;

    int status = pthread_mutex_lock(&pool->mutex);
    if (status != 0)
    {
        free(task);
        return status;
    }

    if (pool->first == NULL)
    {
        pool->first = task;
    }
    else
    {
        pool->last->next = task;
    }
    pool->last = task;

    if (pool->idle_threads > 0)
    {
        status = pthread_cond_signal(&pool->cond);
        if (status != 0)
        {
            pthread_mutex_unlock(&pool->mutex);
            return status;
        }
    }
    else if (pool->current_threads < pool->max_threads)
    {
        pthread_t thread;
        status = pthread_create(&thread, NULL, threadpool_worker, (void*)pool);
        if (status != 0)
        {
            pthread_mutex_unlock(&pool->mutex);
            return status;
        }
        pool->current_threads++;
    }
    pthread_mutex_unlock(&pool->mutex);
    
    return 0;
}

int threadpool_destroy(threadpool_t *pool)
{
    int status = pthread_mutex_lock(&pool->mutex);
    if (status != 0)
    {
        return status;
    }

    if (pool->current_threads > 0)
    {
        pool->quit = true;
        if (pool->idle_threads > 0)
        {
            status = pthread_cond_broadcast(&pool->cond);
            if (status != 0)
            {
                pthread_mutex_unlock(&pool->mutex);
                return status;
            }            
        }

        while (pool->current_threads > 0)
        {
            status = pthread_cond_wait(&pool->cond, &pool->mutex);
            if (status != 0)
            {
                pthread_mutex_unlock(&pool->mutex);
                return status;
            }
        }
    }

    status = pthread_mutex_unlock(&pool->mutex);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);

    return 0;
}

#endif /* THREADPOOL_H */
