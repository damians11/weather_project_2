#include "common.h"
#include "fifo_queue.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *receive_c3_thread(void *arg) {
    while (1) {
        WeatherData data;
        snprintf(data.source, 4, "C3");
        data.temperature = rand() % 20 + 5;
        data.wind_speed = rand() % 10;
        data.solar_radiation = rand() % 100;
        snprintf(data.timestamp, 20, "31/03/2025 14:00");

        enqueue(data);
        sleep(5);
    }
}
