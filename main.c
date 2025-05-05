#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "env_loader.h"
#include "client_connection.h"
#include "receive_c2_thread.h"
#include "receive_c3_thread.h"
#include "fifo_queue.h"
#include "db_writer.h"

int main() {
    init_queue();
    load_env(".env");
    
    pthread_t t_c2, t_c3, t_db;
    pthread_create(&t_c2, NULL, receive_c2_thread, NULL);
    pthread_create(&t_c3, NULL, receive_c3_thread, NULL);
    pthread_create(&t_db, NULL, db_writer_thread, NULL);

    pthread_join(t_c2, NULL);
    pthread_join(t_c3, NULL);
    pthread_join(t_db, NULL);

    return 0;
}
