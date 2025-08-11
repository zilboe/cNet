#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "memory/mempool.h"
#include "thread/thread.h"


static void *ThreadPoolRoutine(void *arg);
static int ThreadPoolCreateThread(CThread_pool_t *pool);
static void ThreadPoolCleanupWorker(worker_t *worker);

int ThreadPoolAddWork(void *pthis, void *(*process)(void *arg), void *arg)
{
    if (!pthis || !process) {
        return -1;
    }
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    
    
    pthread_mutex_lock(&(pool->queue_lock));
    if (pool->shutdown) {
        pthread_mutex_unlock(&(pool->queue_lock));
        return -1;
    }
    
    worker_t *newworker = (worker_t *)cNet_malloc(sizeof(worker_t));
    if (NULL == newworker) {
        pthread_mutex_unlock(&(pool->queue_lock));
        return -1;
    }
    
    newworker->process = process;
    newworker->arg = arg;
    newworker->next = NULL;
    
    if (pool->queue_head != NULL) {
        pool->queue_tail->next = newworker;
    } else {
        pool->queue_head = newworker;
    }
    pool->queue_tail = newworker;
    
    pool->current_wait_task_num++;
    
    if (pool->idle_thread_num == 0 && 
        pool->current_thread_num < pool->max_thread_num) {
        if (ThreadPoolCreateThread(pool) != 0) {
            pool->current_wait_task_num--;
            if (pool->queue_tail == newworker) {
                pool->queue_tail = NULL;
                pool->queue_head = NULL;
            } else {
                worker_t *prev = pool->queue_head;
                while (prev && prev->next != newworker) {
                    prev = prev->next;
                }
                if (prev) {
                    prev->next = NULL;
                    pool->queue_tail = prev;
                }
            }
            pthread_mutex_unlock(&(pool->queue_lock));
            free(newworker);
            return -1;
        }
    }
    
    pthread_mutex_unlock(&(pool->queue_lock));
    
    pthread_cond_signal(&(pool->queue_ready));
    
    return 0;
}

int ThreadPoolAddWorkAndWait(void *pthis, void *(*process)(void *arg), void *arg)
{
    if (!pthis || !process) {
        return -1;
    }
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    
    pthread_mutex_lock(&(pool->queue_lock));
    if (pool->shutdown) {
        pthread_mutex_unlock(&(pool->queue_lock));
        return -1;
    }
    
    worker_t *newworker = (worker_t *)malloc(sizeof(worker_t));
    if (NULL == newworker) {
        pthread_mutex_unlock(&(pool->queue_lock));
        return -1;
    }
    
    newworker->process = process;
    newworker->arg = arg;
    newworker->next = NULL;
    
    if (pool->queue_head != NULL) {
        pool->queue_tail->next = newworker;
    } else {
        pool->queue_head = newworker;
    }
    pool->queue_tail = newworker;
    
    pool->current_wait_task_num++;
    pool->wait_for_task_empty = 1;
    
    if (pool->idle_thread_num == 0 && 
        pool->current_thread_num < pool->max_thread_num) {
        if (ThreadPoolCreateThread(pool) != 0) {
            pool->current_wait_task_num--;
            pool->wait_for_task_empty = 0;
            if (pool->queue_tail == newworker) {
                pool->queue_tail = NULL;
                pool->queue_head = NULL;
            }
            pthread_mutex_unlock(&(pool->queue_lock));
            free(newworker);
            return -1;
        }
    }
    
    
    while (pool->current_wait_task_num > 0 && !pool->shutdown) {
        pthread_cond_wait(&(pool->queue_empty), &(pool->queue_lock));
    }
    
    pool->wait_for_task_empty = 0;
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));
    
    return 0;
}

int ThreadPoolGetThreadNum(void *pthis)
{
    if (!pthis) return -1;
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    pthread_mutex_lock(&(pool->queue_lock));
    int num = pool->current_thread_num;
    pthread_mutex_unlock(&(pool->queue_lock));
    
    return num;
}

int ThreadPoolGetTaskNum(void *pthis)
{
    if (!pthis) return -1;
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    pthread_mutex_lock(&(pool->queue_lock));
    int num = pool->current_wait_task_num;
    pthread_mutex_unlock(&(pool->queue_lock));
    
    return num;
}

int ThreadPoolGetRunningTaskNum(void *pthis)
{
    if (!pthis) return -1;
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    pthread_mutex_lock(&(pool->queue_lock));
    int num = pool->current_running_task_num;
    pthread_mutex_unlock(&(pool->queue_lock));
    
    return num;
}

int ThreadPoolDestroy(void *pthis)
{
    if (!pthis) return -1;
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    
    pthread_mutex_lock(&(pool->queue_lock));
    if (pool->shutdown) {
        pthread_mutex_unlock(&(pool->queue_lock));
        return -1;
    }
    
    pool->shutdown = 1;
    pthread_mutex_unlock(&(pool->queue_lock));
    
    
    pthread_cond_broadcast(&(pool->queue_ready));
    
    return 0;
}


int ThreadPoolDestroyAndWait(void *pthis)
{
    if (!pthis) return -1;
    
    CThread_pool_t *pool = (CThread_pool_t *)pthis;
    
    pthread_mutex_lock(&(pool->queue_lock));
    if (pool->shutdown) {
        pthread_mutex_unlock(&(pool->queue_lock));
        return -1;
    }
    
    pool->shutdown = 1;
    pthread_mutex_unlock(&(pool->queue_lock));
    
    
    pthread_cond_broadcast(&(pool->queue_ready));
    
   
    for (int i = 0; i < pool->current_thread_num; i++) {
        pthread_join(pool->threadid[i], NULL);
    }
    
    
    free(pool->threadid);
    
  
    worker_t *head = NULL;
    while (pool->queue_head != NULL) {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        ThreadPoolCleanupWorker(head);
    }
    
    
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    pthread_cond_destroy(&(pool->queue_empty));
    
    free(pool);
    
    return 0;
}

static void *ThreadPoolRoutine(void *arg)
{
    CThread_pool_t *pool = (CThread_pool_t *)arg;
    
    while (1) {
        pthread_mutex_lock(&(pool->queue_lock));
        
        while (pool->current_wait_task_num == 0 && !pool->shutdown) {
            pool->idle_thread_num++;
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
            pool->idle_thread_num--;
        }
        

        if (pool->shutdown) {
            pool->current_thread_num--;
            pthread_mutex_unlock(&(pool->queue_lock));
            pthread_exit(NULL);
        }

        worker_t *worker = pool->queue_head;
        if (worker != NULL) {
            pool->queue_head = worker->next;
            if (pool->queue_head == NULL) {
                pool->queue_tail = NULL;
            }
            pool->current_wait_task_num--;
            pool->current_running_task_num++;
        }
        
        pthread_mutex_unlock(&(pool->queue_lock));
        
        if (worker != NULL) {
            (*(worker->process))(worker->arg);
            
            pthread_mutex_lock(&(pool->queue_lock));
            pool->current_running_task_num--;
            
            if (pool->wait_for_task_empty && pool->current_wait_task_num == 0) {
                pthread_cond_broadcast(&(pool->queue_empty));
            }
            
            pthread_mutex_unlock(&(pool->queue_lock));
            ThreadPoolCleanupWorker(worker);
        }
        
        pthread_mutex_lock(&(pool->queue_lock));
        if (pool->current_thread_num > pool->min_thread_num && 
            pool->idle_thread_num > pool->free_thread_num &&
            pool->current_thread_num > pool->min_thread_num) {
            if (pool->current_thread_num > pool->min_thread_num) {
                pool->current_thread_num--;
                pthread_mutex_unlock(&(pool->queue_lock));
                break;
            }
        }
        pthread_mutex_unlock(&(pool->queue_lock));
    }
    
    pthread_exit(NULL);
    return NULL;
}

static int ThreadPoolCreateThread(CThread_pool_t *pool)
{
    if (!pool) return -1;
    
    pthread_t *new_threadid = (pthread_t *)realloc(pool->threadid, 
                                                   (pool->current_thread_num + 1) * sizeof(pthread_t));
    if (new_threadid == NULL) {
        return -1;
    }
    
    pool->threadid = new_threadid;
    
    if (pthread_create(&(pool->threadid[pool->current_thread_num]), 
                       NULL, ThreadPoolRoutine, (void *)pool) != 0) {
        return -1;
    }
    
    pool->current_thread_num++;
    
    return 0;
}

static void ThreadPoolCleanupWorker(worker_t *worker)
{
    if (worker) {
        free(worker);
    }
}

CThread_pool_t *ThreadPoolConstruct(int max_num, int min_num, int free_num)
{
    CThread_pool_t *pool = (CThread_pool_t *)malloc(sizeof(CThread_pool_t));
    if (NULL == pool) {
        return NULL;
    }
    
    memset(pool, 0, sizeof(CThread_pool_t));
    
    if (pthread_mutex_init(&(pool->queue_lock), NULL) != 0) {
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&(pool->queue_ready), NULL) != 0) {
        pthread_mutex_destroy(&(pool->queue_lock));
        free(pool);
        return NULL;
    }
    
    if (pthread_cond_init(&(pool->queue_empty), NULL) != 0) {
        pthread_mutex_destroy(&(pool->queue_lock));
        pthread_cond_destroy(&(pool->queue_ready));
        free(pool);
        return NULL;
    }
    
    pool->max_thread_num = max_num > 0 ? max_num : DEFAULT_MAX_THREAD_NUM;
    pool->min_thread_num = min_num > 0 ? min_num : DEFAULT_MIN_THREAD_NUM;
    pool->free_thread_num = free_num > 0 ? free_num : DEFAULT_FREE_THREAD_NUM;
    
    if (pool->min_thread_num > pool->max_thread_num) {
        pool->min_thread_num = pool->max_thread_num;
    }
    
    pool->current_thread_num = 0;
    pool->idle_thread_num = 0;
    pool->current_wait_task_num = 0;
    pool->current_running_task_num = 0;
    pool->shutdown = 0;
    pool->wait_for_task_empty = 0;
    pool->queue_head = NULL;
    pool->queue_tail = NULL;
    pool->threadid = NULL;
    
    pool->AddWork = ThreadPoolAddWork;
    pool->AddWorkAndWait = ThreadPoolAddWorkAndWait;
    pool->Destroy = ThreadPoolDestroy;
    pool->DestroyAndWait = ThreadPoolDestroyAndWait;
    pool->GetThreadNum = ThreadPoolGetThreadNum;
    pool->GetTaskNum = ThreadPoolGetTaskNum;
    pool->GetRunningTaskNum = ThreadPoolGetRunningTaskNum;
    
    for (int i = 0; i < pool->min_thread_num; i++) {
        if (ThreadPoolCreateThread(pool) != 0) {
            pool->shutdown = 1;
            pthread_cond_broadcast(&(pool->queue_ready));
            for (int j = 0; j < pool->current_thread_num; j++) {
                pthread_join(pool->threadid[j], NULL);
            }
            if (pool->threadid) free(pool->threadid);
            pthread_mutex_destroy(&(pool->queue_lock));
            pthread_cond_destroy(&(pool->queue_ready));
            pthread_cond_destroy(&(pool->queue_empty));
            free(pool);
            return NULL;
        }
    }
    
    return pool;
}


CThread_pool_t *ThreadPoolConstructDefault(void)
{
    return ThreadPoolConstruct(DEFAULT_MAX_THREAD_NUM, 
                              DEFAULT_MIN_THREAD_NUM, 
                              DEFAULT_FREE_THREAD_NUM);
}