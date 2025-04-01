#include "fifo_queue.h"
#include <pthread.h>

#define MAX_QUEUE 100
static WeatherData queue[MAX_QUEUE];
static int front = 0, rear = 0;
static pthread_mutex_t mutex;

void init_queue() {
    pthread_mutex_init(&mutex, NULL);
}

int is_queue_empty() {
    return front == rear;
}

int enqueue(WeatherData data) {
    pthread_mutex_lock(&mutex);
    if ((rear + 1) % MAX_QUEUE == front) {
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    queue[rear] = data;
    rear = (rear + 1) % MAX_QUEUE;
    pthread_mutex_unlock(&mutex);
    return 1;
}

int dequeue(WeatherData *data) {
    pthread_mutex_lock(&mutex);
    if (is_queue_empty()) {
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    *data = queue[front];
    front = (front + 1) % MAX_QUEUE;
    pthread_mutex_unlock(&mutex);
    return 1;
}
