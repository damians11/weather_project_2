#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct {
    char source[4];
    char timestamp[20];
    double temperature;
    double wind_speed;
    int solar_radiation;
} WeatherData;

#endif
