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
    pthread_cond_t quit_cond;
    threadpool_task_t *first;
    threadpool_task_t *last;
    int max_threads;
    int current_threads;
    int idle_threads;
    bool quit;
} threadpool_t;

/* 
    Internal function:
    Get task form the linkedlist
 */
static threadpool_task_t *threadpool_get_task(threadpool_t *pool)
{
    if (pool != NULL && pool->first != NULL)
    {
        threadpool_task_t *task = pool->first;
        pool->first = task->next;
        if (pool->first == NULL)
        {
            pool->last = NULL;
        }
        return task;
    }
    return NULL;
}

/*
    Internal function:
    Add the task node to the linkedlist
 */
static void threadpool_add_task(threadpool_t *pool, threadpool_task_t *task)
{
    if (pool != NULL)
    {
        if (pool->first == NULL)
        {
            pool->first = task;
        }
        else
        {
            pool->last->next = task;
        }
        pool->last = task;
    }
}

static void *threadpool_worker(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;

    struct timeval now;
    struct timespec timeout;

    int status = pthread_mutex_lock(&pool->mutex);
    if (status != 0)
    {
        return NULL;
    }
    
    while (true)
    {
        // whether the current thread has timed out
        // while waiting on the condition variable
        bool has_timedout = false;

        // current thread idle now
        pool->idle_threads++;
        
        while (pool->first == NULL || !pool->quit)
        {
            gettimeofday(&now, NULL);
            timeout.tv_sec = now.tv_sec + 2;
            timeout.tv_nsec = now.tv_usec * 1000;
            
            status = pthread_cond_timedwait(&pool->cond, &pool->mutex, &timeout);
            if (status == ETIMEDOUT)
            {
                has_timedout = true;
                break;
            }
        }

        // current thread busy now
        pool->idle_threads--;
        
        if (pool->quit)
        {
            pool->current_threads--;
            
            // notify this thread will quit now
            pthread_cond_broadcast(&pool->quit_cond);
            break;
        }

        threadpool_task_t *task = threadpool_get_task(pool);        
        if (task != NULL)
        {
            pthread_mutex_unlock(&pool->mutex);
            task->function(task->argument);
            free(task);

            status = pthread_mutex_lock(&pool->mutex);
            if (status != 0)
            {
                return NULL;
            }
        }

        if (pool->first == NULL && has_timedout)
        {
            // we will quit form this thread
            pool->current_threads--;
            break;
        }        
    }

    pthread_mutex_unlock(&pool->mutex);    
    return NULL;
}

/*
    Create the thread pool
 */
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

    status = pthread_cond_init(&pool->quit_cond, NULL);
    if (status != 0)
    {
        pthread_cond_destroy(&pool->cond);
        pthread_mutex_destroy(&pool->mutex);
        return status;
    }
    
    pool->first = NULL;
    pool->last = NULL;
    pool->max_threads = max_threads;
    pool->current_threads = 0;
    pool->idle_threads = 0;
    pool->quit = false;

    return 0;
}


/*
    Add the task in thread pool for executing
 */
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
    threadpool_add_task(pool, task);

    if (pool->idle_threads > 0)
    {
        // we have some idle threads, just wake up one
        status = pthread_cond_signal(&pool->cond);
        if (status != 0)
        {
            pthread_mutex_unlock(&pool->mutex);
            return status;
        }
    }
    else if (pool->current_threads < pool->max_threads)
    {
        // all threads are busy, we create the new one
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
            // wake up all the threads
            status = pthread_cond_broadcast(&pool->cond);
            if (status != 0)
            {
                pthread_mutex_unlock(&pool->mutex);
                return status;
            }            
        }
        
        while (pool->current_threads > 0)
        {
            status = pthread_cond_wait(&pool->quit_cond, &pool->mutex);
            if (status != 0)
            {
                pthread_mutex_unlock(&pool->mutex);
                return status;
            }
        }
    }

    pthread_mutex_unlock(&pool->mutex);
    pthread_cond_destroy(&pool->quit_cond);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    
    return 0;
}


#endif /* THREADPOOL_H */
