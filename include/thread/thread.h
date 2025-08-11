#ifndef THREAD_H
#define	THREAD_H
#include <pthread.h>

#include <pthread.h>
#include <stdint.h>

#define DEFAULT_MAX_THREAD_NUM 100
#define DEFAULT_FREE_THREAD_NUM 10
#define DEFAULT_MIN_THREAD_NUM 10

typedef struct worker {
    void *(*process)(void *arg);
    void *arg;
    struct worker *next;
} worker_t;


typedef struct {
    pthread_mutex_t queue_lock;     
    pthread_cond_t queue_ready;     
    pthread_cond_t queue_empty;     
    
    worker_t *queue_head;          
    worker_t *queue_tail;           
    
    pthread_t *threadid;            
    
    int max_thread_num;             
    int min_thread_num;             
    int current_thread_num;         
    int idle_thread_num;            
    int free_thread_num;            
    
    int current_wait_task_num;     
    int current_running_task_num;   
    
    int shutdown;                  
    int wait_for_task_empty;        
    
    
    int (*AddWork)(void *pthis, void *(*process)(void *arg), void *arg);
    int (*AddWorkAndWait)(void *pthis, void *(*process)(void *arg), void *arg);
    int (*Destroy)(void *pthis);
    int (*DestroyAndWait)(void *pthis);
    int (*GetThreadNum)(void *pthis);
    int (*GetTaskNum)(void *pthis);
    int (*GetRunningTaskNum)(void *pthis);
    
} CThread_pool_t;


CThread_pool_t *ThreadPoolConstruct(int max_num, int min_num, int free_num);
CThread_pool_t *ThreadPoolConstructDefault(void);

#endif

