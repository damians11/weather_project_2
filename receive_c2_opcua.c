#include "common.h"
#include "fifo_queue.h"
#include <open62541/client.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/client_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/types_generated.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *receive_c2_thread(void *arg) {
    const char *server_url = getenv("C2_SERVER_URL");
    if (!server_url) {
        printf("[C2 OPCUA] Brak C2_SERVER_URL w .env\n");
        return NULL;
    }

    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode status = UA_Client_connect(client, server_url);
    if (status != UA_STATUSCODE_GOOD) {
        printf("[C2 OPCUA] B??d po??czenia: %s\n", UA_StatusCode_name(status));
        UA_Client_delete(client);
        return NULL;
    }

    printf("[C2 OPCUA] Po??czono z serwerem: %s\n", server_url);

    while (1) {
        UA_Variant value;
        UA_Variant_init(&value);

        status = UA_Client_readValueAttribute(client, UA_NODEID_NUMERIC(1, 2201), &value);

        printf("[C2 OPCUA] Odczyt node 2201 ? status: %s\n", UA_StatusCode_name(status));

        if (status == UA_STATUSCODE_GOOD &&
            value.arrayLength > 0 &&
            value.type == &UA_TYPES[UA_TYPES_FLOAT]) {
            
            float *arr = (float *)value.data;

            
            printf("[C2 OPCUA] Odebrano tablic?. Pierwsza warto??: %.2f\n", arr[0]);

            WeatherData data;
            snprintf(data.source, sizeof(data.source), "C2");
            data.temperature = arr[0];
            data.wind_speed = arr[1];
            data.solar_radiation = arr[2];
            snprintf(data.timestamp, sizeof(data.timestamp), "01/04/2025 18:00");

            enqueue(data);
        } else {
            printf("[C2 OPCUA] Dane jeszcze nie gotowe lub niew?a?ciwe ? czekam...\n");
        }

        sleep(10);
    }

    UA_Client_disconnect(client);
    UA_Client_delete(client);
    return NULL;
}
