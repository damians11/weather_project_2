#ifndef FIFO_QUEUE_H
#define FIFO_QUEUE_H

#include "common.h"

void init_queue();
int enqueue(WeatherData data);
int dequeue(WeatherData *data);
int is_queue_empty();

#endif
