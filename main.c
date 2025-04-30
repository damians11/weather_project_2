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

    /*const char *server_url = getenv("C2_SERVER_URL"); // Wspolny URL dla C2 i C3
    if (!server_url) {
        fprintf(stderr, "[MAIN] Brak zmiennej C2_SERVER_URL w .env\n");
        return 1;
    }

    global_opcua_client = create_opcua_client(server_url);
    if (!global_opcua_client) {
        fprintf(stderr, "[MAIN] Nie udalo sie polaczyc z serwerem OPC UA\n");
        return 1;
    }*/
    
    pthread_t t_c2, t_c3, t_db;
    pthread_create(&t_c2, NULL, receive_c2_thread, NULL);
    pthread_create(&t_c3, NULL, receive_c3_thread, NULL);
    pthread_create(&t_db, NULL, db_writer_thread, NULL);

    pthread_join(t_c2, NULL);
    pthread_join(t_c3, NULL);
    pthread_join(t_db, NULL);

    return 0;
}
