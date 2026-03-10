#pragma once

#include <pthread.h>

#include "types.h"
#include "scene.h"

typedef struct AppContext AppContext;

typedef struct {
    void (*function)(void *arg);
    void *arg;
} Job;

typedef struct ThreadPool {
    pthread_cond_t wake_condition;   // 48 bytes
    pthread_cond_t sleep_condition;  // 48 bytes
    pthread_mutex_t mutex;           // 40 bytes
    pthread_t *threads;              // 8 bytes
    Job *job_queue;                  // 8 bytes
    int tasks_in_progress;           // 4 bytes
    int thread_count;                // 4 bytes
    int job_queue_size;              // 4 bytes
    int job_queue_capacity;          // 4 bytes
    int head_index;                  // 4 bytes
    int tail_index;                  // 4 bytes
    bool stop_flag;                  // 1 byte
} ThreadPool;

void* thread_pool_worker_function(void *arg);
void initialize_thread_pool(ThreadPool *thread_pool, int thread_count);
void destroy_thread_pool(ThreadPool *thread_pool);
void submit_job_to_thread_pool(ThreadPool *thread_pool, void (*function)(void *), void *arg);
void thread_pool_wait_for_completion(ThreadPool *thread_pool);