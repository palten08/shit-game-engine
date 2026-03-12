#include "../include/multithreading.h"
#include "../include/types.h"
#include "../include/vector_operations.h"
#include "../include/matrix_operations.h"
#include "../include/clipping.h"
#include "../include/coordinates.h"
#include "../include/ecs.h"
#include "../include/logging.h"

void* thread_pool_worker_function(void *arg) {
    ThreadPool *pool = (ThreadPool*)arg;
    while (true) {
        // lock the mutex
        pthread_mutex_lock(&pool->mutex);

        while (!pool->stop_flag && pool->job_queue_size == 0) {
            // Wait for jobs
            pthread_cond_wait(&pool->wake_condition, &pool->mutex);
        }
        // Pop a job from the ring buffer
        if (pool->job_queue_size > 0) {
            Job job = pool->job_queue[pool->head_index];
            pool->head_index = (pool->head_index + 1) % pool->job_queue_capacity;
            pool->job_queue_size--;
            // Unlock the mutex before executing the job
            pthread_mutex_unlock(&pool->mutex);
            // Execute the job
            job.function(job.arg);
            // Lock the mutex again to update tasks_in_progress
            pthread_mutex_lock(&pool->mutex);
            pool->tasks_in_progress--;
            if (pool->tasks_in_progress == 0) {
                pthread_cond_signal(&pool->sleep_condition);
            }
            pthread_mutex_unlock(&pool->mutex);
            // If there are no more tasks in progress, signal any waiting threads
        } else if (pool->stop_flag) {
            // If stop_flag is set and there are no jobs, exit the thread
            pthread_mutex_unlock(&pool->mutex);
            return NULL;

        }
    }
}

void initialize_thread_pool(ThreadPool *thread_pool, int thread_count) {
    LOG_INFO("Initializing thread pool with %d threads", thread_count);
    thread_pool->threads = malloc(thread_count * sizeof(pthread_t));
    if (!thread_pool->threads) {
        LOG_ERROR("Failed to allocate memory for thread pool");
        exit(EXIT_FAILURE);
    }
    thread_pool->thread_count = thread_count;
    thread_pool->job_queue_size = 0;
    thread_pool->job_queue_capacity = 16; // Initial capacity for the job queue
    thread_pool->job_queue = malloc(thread_pool->job_queue_capacity * sizeof(Job));
    if (!thread_pool->job_queue) {
        LOG_ERROR("Failed to allocate memory for job queue");
        exit(EXIT_FAILURE);
    }
    thread_pool->tasks_in_progress = 0;
    thread_pool->head_index = 0;
    thread_pool->tail_index = 0;
    thread_pool->stop_flag = false;
    pthread_mutex_init(&thread_pool->mutex, NULL);
    pthread_cond_init(&thread_pool->wake_condition, NULL);
    pthread_cond_init(&thread_pool->sleep_condition, NULL);
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&thread_pool->threads[i], NULL, thread_pool_worker_function, thread_pool);
    }
}

void destroy_thread_pool(ThreadPool *thread_pool) {
    LOG_INFO("Destroying thread pool");
    // Signal all threads to stop
    pthread_mutex_lock(&thread_pool->mutex);
    thread_pool->stop_flag = true;
    pthread_cond_broadcast(&thread_pool->wake_condition);
    pthread_mutex_unlock(&thread_pool->mutex);
    // Wait for all threads to finish
    for (int i = 0; i < thread_pool->thread_count; i++) {
        pthread_join(thread_pool->threads[i], NULL);
    }
    // Clean up resources
    free(thread_pool->threads);
    free(thread_pool->job_queue);
    pthread_mutex_destroy(&thread_pool->mutex);
    pthread_cond_destroy(&thread_pool->wake_condition);
    pthread_cond_destroy(&thread_pool->sleep_condition);
}

void submit_job_to_thread_pool(ThreadPool *thread_pool, void (*function)(void *), void *arg) {
    pthread_mutex_lock(&thread_pool->mutex);
    // Resize the job queue if it's full
    if (thread_pool->job_queue_size == thread_pool->job_queue_capacity) {
        thread_pool->job_queue_capacity *= 2;
        thread_pool->job_queue = realloc(thread_pool->job_queue, thread_pool->job_queue_capacity * sizeof(Job));
        if (!thread_pool->job_queue) {
            LOG_ERROR("Failed to reallocate memory for job queue");
            exit(EXIT_FAILURE);
        }
        // If the tail index is behind the head index, we need to move the jobs that are wrapped around to the end of the new buffer
        if (thread_pool->tail_index <= thread_pool->head_index) {
            memmove(&thread_pool->job_queue[thread_pool->head_index + thread_pool->job_queue_capacity / 2], &thread_pool->job_queue[thread_pool->head_index], (thread_pool->job_queue_capacity / 2 - thread_pool->head_index) * sizeof(Job));
            thread_pool->head_index += thread_pool->job_queue_capacity / 2;
        }
    }
    // Add the new job to the ring buffer
    thread_pool->job_queue[thread_pool->tail_index] = (Job){.function = function, .arg = arg};
    thread_pool->tail_index = (thread_pool->tail_index + 1) % thread_pool->job_queue_capacity;
    thread_pool->job_queue_size++;
    thread_pool->tasks_in_progress++;

    // Signal a worker thread that a new job is available
    pthread_cond_signal(&thread_pool->wake_condition);
    pthread_mutex_unlock(&thread_pool->mutex);
}

void thread_pool_wait_for_completion(ThreadPool *thread_pool) {
    pthread_mutex_lock(&thread_pool->mutex);
    while (thread_pool->tasks_in_progress > 0) {
        pthread_cond_wait(&thread_pool->sleep_condition, &thread_pool->mutex);
    }
    pthread_mutex_unlock(&thread_pool->mutex);
}
