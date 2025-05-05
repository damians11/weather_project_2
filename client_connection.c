#include "client_connection.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

UA_Client *create_opcua_client(const char *url) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    if (UA_Client_connect(client, url) != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return NULL;
    }

    return client;
}

