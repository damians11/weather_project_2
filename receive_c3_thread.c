#include "receive_c3_thread.h"
#include "fifo_queue.h"
#include "global.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void *receive_c3_thread(void *arg) {
    while (1) {
        WeatherData data;
        strncpy(data.source, "C3", sizeof(data.source));
        snprintf(data.timestamp, sizeof(data.timestamp), "01/04/2025 12:00");
        data.temperature = 20.5;
        data.wind_speed = 3.1;
        data.solar_radiation = 100;

        enqueue(data);
        sleep(1000);
    }
    return NULL;
}
