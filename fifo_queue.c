#include "fifo_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_SIZE 128

static WeatherData queue[QUEUE_SIZE];
static int front = 0, rear = 0, count = 0;
static pthread_mutex_t queue_mutex;

void init_queue() {
    pthread_mutex_init(&queue_mutex, NULL);
}

int enqueue(WeatherData data) {
    pthread_mutex_lock(&queue_mutex);
    if (count < QUEUE_SIZE) {
        queue[rear] = data;
        rear = (rear + 1) % QUEUE_SIZE;
        count++;
        pthread_mutex_unlock(&queue_mutex);
        return 0;  // sukces
    } else {
        pthread_mutex_unlock(&queue_mutex);
        return -1; // kolejka pelna
    }
}

int dequeue(WeatherData *data) {
    pthread_mutex_lock(&queue_mutex);
    if (count == 0) {
        pthread_mutex_unlock(&queue_mutex);
        return 0;
    }
    *data = queue[front];
    front = (front + 1) % QUEUE_SIZE;
    count--;
    pthread_mutex_unlock(&queue_mutex);
    return 1;
}

int is_queue_empty() {
    pthread_mutex_lock(&queue_mutex);
    int empty = (count == 0);
    pthread_mutex_unlock(&queue_mutex);
    return empty;
}
