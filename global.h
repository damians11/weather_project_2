#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct {
    char source[4];              // "C2" lub "C3"

    char timestamp_c2[20];       // Czas z danych C2
    char timestamp_c3[20];       // Czas z danych C3

    // Dla C2
    float temperature;
    float wind_speed;
    int solar_radiation;

    // Dla C3 (dane energetyczne)
    double cieplne;
    double wodne;
    double wiatrowe;
    double fotowoltaiczne;
    double inne;
    double cz, de, sk, lt, ua, se;
    double frequency;
} WeatherData;

#endif

