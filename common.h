#ifndef COMMON_H
#define COMMON_H

typedef struct {
    char source[4];
    double temperature;
    double wind_speed;
    double solar_radiation;
    char timestamp[20];
} WeatherData;

#endif
