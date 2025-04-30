#include "client_connection.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

//UA_Client *global_opcua_client = NULL; 

UA_Client *create_opcua_client(const char *url) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    if (UA_Client_connect(client, url) != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return NULL;
    }

    return client;
}

/*
UA_Client *global_opcua_client = NULL;
static pthread_t opcua_loop_thread_id;

static void *opcua_loop(void *arg) {
    while (1) {
        UA_Client_run_iterate(global_opcua_client, 100);
        usleep(100000);  // 100 ms
    }
    return NULL;
}

UA_Client *create_opcua_client(const char *url) {
    global_opcua_client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(global_opcua_client));

    if (UA_Client_connect(global_opcua_client, url) != UA_STATUSCODE_GOOD) {
        UA_Client_delete(global_opcua_client);
        global_opcua_client = NULL;
        return NULL;
    }

    
    pthread_create(&opcua_loop_thread_id, NULL, opcua_loop, NULL);

    return global_opcua_client;
}
*/
