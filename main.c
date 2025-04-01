#include <pthread.h>
#include "common.h"
#include "fifo_queue.h"
#include "db_writer.h"
#include "receive_c2_opcua.h"
#include "receive_c3_thread.h"

int main() {
    init_queue();

    pthread_t tid_c2, tid_c3, tid_db;
    pthread_create(&tid_c2, NULL, receive_c2_thread, NULL);
    pthread_create(&tid_c3, NULL, receive_c3_thread, NULL);
    pthread_create(&tid_db, NULL, db_writer_thread, NULL);

    pthread_join(tid_c2, NULL);
    pthread_join(tid_c3, NULL);
    pthread_join(tid_db, NULL);

    return 0;
}
