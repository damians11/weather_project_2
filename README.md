# weather_project_2

gcc -o app main.c fifo_queue.c db_writer.c receive_c2_thread.c receive_c3_thread.c client_connection.c env_loader.c /usr/local/lib/libopen62541.a -I/usr/local/include -I/usr/include/mariadb -L/usr/lib/aarch64-linux-gnu -lmariadb -lpthread