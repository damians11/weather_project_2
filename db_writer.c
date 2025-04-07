#define _XOPEN_SOURCE 700
#include "global.h"
#include "fifo_queue.h"
#include <mariadb/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BACKUP_FILE "backup_log.txt"

void *db_writer_thread(void *arg) {
    MYSQL *conn;
    WeatherData data;

    conn = mysql_init(NULL);
    mysql_real_connect(conn, getenv("DB_HOST"), getenv("DB_USER"), getenv("DB_PASS"),
                       getenv("DB_NAME"), 3306, NULL, 0);

    while (1) {
        if (dequeue(&data)) {
            char mysql_timestamp[20];
            struct tm tm;

            // Konwersja daty z "DD/MM/YYYY HH:MM" na "YYYY-MM-DD HH:MM:SS"
            if (strptime(data.timestamp, "%d/%m/%Y %H:%M", &tm)) {
                strftime(mysql_timestamp, sizeof(mysql_timestamp), "%Y-%m-%d %H:%M:%S", &tm);
            } else {
                fprintf(stderr, "[TIME ERROR] Niepoprawny format daty: %s\n", data.timestamp);
                continue;
            }

            // Budowanie zapytania SQL
            char query[512];
            snprintf(query, sizeof(query),
                     "INSERT INTO weather_data (source, timestamp, temperature, wind_speed, solar_radiation) "
                     "VALUES ('%s', '%s', %.2f, %.2f, %d)",
                     data.source, mysql_timestamp, data.temperature,
                     data.wind_speed, data.solar_radiation);

            // Zapis do bazy
            if (mysql_query(conn, query)) {
                fprintf(stderr, "[DB ERROR] %s\n", mysql_error(conn));
            }

            // Zapis do pliku tekstowego (kopii zapasowej)
            FILE *file = fopen(BACKUP_FILE, "a");
            if (file) {
                fprintf(file, "%s;%s;%.2f;%.2f;%d\n",
                        data.source, mysql_timestamp, data.temperature,
                        data.wind_speed, data.solar_radiation);
                fclose(file);
            } else {
                fprintf(stderr, "[LOG ERROR] Nie mo?na otworzy? pliku %s\n", BACKUP_FILE);
            }

        } else {
            sleep(1); // 1 s
        }
    }

    mysql_close(conn);
    return NULL;
}
