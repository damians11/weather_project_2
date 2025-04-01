#include "db_writer.h"
#include "common.h"
#include "fifo_queue.h"
#include <mariadb/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *db_writer_thread(void *arg) {
    while (1) {
        if (!is_queue_empty()) {
            WeatherData data;
            if (dequeue(&data)) {
                MYSQL *conn = mysql_init(NULL);
                mysql_real_connect(conn, getenv("DB_HOST"), getenv("DB_USER"), getenv("DB_PASS"),
                                   getenv("DB_NAME"), 3306, NULL, 0);

                char query[512];
                snprintf(query, sizeof(query),
                         "INSERT INTO weather_log (source, temperature, wind_speed, solar_radiation, timestamp) "
                         "VALUES ('%s', %.2f, %.2f, %.2f, '%s')",
                         data.source, data.temperature, data.wind_speed, data.solar_radiation, data.timestamp);

                if (mysql_query(conn, query)) {
                    fprintf(stderr, "[DB ERROR] %s\n", mysql_error(conn));
                }

                mysql_close(conn);
            }
        }
        sleep(5);
    }
    return NULL;
}
