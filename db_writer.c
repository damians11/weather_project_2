#define _XOPEN_SOURCE 700
#include "global.h"
#include "fifo_queue.h"
#include <mariadb/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BACKUP_FILE_C2 "backup_log_c2.txt"
#define BACKUP_FILE_C3 "backup_log_c3.txt"

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
            const char *src = data.source;

            const char *input_time = strcmp(src, "C2") == 0 ? data.timestamp_c2 : data.timestamp_c3;
            const char *backup_file = strcmp(src, "C2") == 0 ? BACKUP_FILE_C2 : BACKUP_FILE_C3;

            if (strptime(input_time, "%d/%m/%Y %H:%M", &tm)) {
                strftime(mysql_timestamp, sizeof(mysql_timestamp), "%Y-%m-%d %H:%M:%S", &tm);
            } else {
                fprintf(stderr, "[TIME ERROR] Niepoprawny format daty: %s\n", input_time);
                continue;
            }
            
            if (mysql_ping(conn)){
                fprintf(stderr, "[DB] zerwane polaczenie, nawiazuje ponownie...\n");
                mysql_close(conn);
                conn = mysql_init(NULL);
                mysql_real_connect(conn, getenv("DB_HOST"), getenv("DB_USER"), getenv("DB_PASS"),
                                   getenv("DB_NAME"), 3306, NULL, 0);
            }

            char query[1024];
            if (strcmp(src, "C2") == 0) {
                snprintf(query, sizeof(query),
                         "INSERT INTO weather_data (source, timestamp, temperature, wind_speed, solar_radiation) "
                         "VALUES ('%s', '%s', %.2f, %.2f, %d)",
                         data.source, mysql_timestamp, data.temperature,
                         data.wind_speed, data.solar_radiation);
            } else if (strcmp(src, "C3") == 0) {
                snprintf(query, sizeof(query),
                         "INSERT INTO energy_data (source, timestamp, cieplne, wodne, wiatrowe, fotowoltaiczne, inne, cz, de, sk, lt, ua, se, frequency) "
                         "VALUES ('%s', '%s', %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)",
                         data.source, mysql_timestamp, data.cieplne, data.wodne, data.wiatrowe,
                         data.fotowoltaiczne, data.inne, data.cz, data.de, data.sk, data.lt,
                         data.ua, data.se, data.frequency);
            } else {
                fprintf(stderr, "[DB ERROR] Nieznane zrodlo danych: %s\n", src);
                continue;
            }

            if (mysql_query(conn, query)) {
                fprintf(stderr, "[DB ERROR] %s\n", mysql_error(conn));
            }

            FILE *file = fopen(backup_file, "a");
            if (file) {
                if (strcmp(src, "C2") == 0) {
                    fprintf(file, "%s;%s;%.2f;%.2f;%d\n",
                            data.source, mysql_timestamp, data.temperature,
                            data.wind_speed, data.solar_radiation);
                } else if (strcmp(src, "C3") == 0) {
                    fprintf(file, "%s;%s;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\n",
                            data.source, mysql_timestamp, data.cieplne, data.wodne, data.wiatrowe,
                            data.fotowoltaiczne, data.inne, data.cz, data.de, data.sk,
                            data.lt, data.ua, data.se, data.frequency);
                }
                fclose(file);
            } else {
                fprintf(stderr, "[LOG ERROR] Nie mozna otworzyc pliku %s\n", backup_file);
            }

        } else {
            sleep(1);
        }
    }

    mysql_close(conn);
    return NULL;
}
