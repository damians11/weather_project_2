#include "receive_c2_thread.h"
#include "client_connection.h"
#include "fifo_queue.h"
#include "global.h"
#include "env_loader.h"
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
#include <time.h>

static time_t time_received_c2 = 0;
static int wait_flag_c2 = 1;
static WeatherData currentData;
static int has_time_c2 = 0, has_temp = 0, has_wind = 0, has_cloud = 0;

static void try_enqueue() {
    if ((has_time_c2 && has_temp && has_wind && has_cloud) || wait_flag_c2) {
        enqueue(currentData);
        //memset(&currentData, 0, sizeof(currentData));
        wait_flag_c2 = 0;
        has_time_c2 = has_temp = has_wind = has_cloud = 0;
    }
}

static void handle_time_c2(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {
        strncpy(currentData.source, "C2", sizeof(currentData.source));
        snprintf(currentData.timestamp_c2, sizeof(currentData.timestamp_c2), "%.*s",
                 (int)((UA_String *)value->value.data)->length,
                 ((UA_String *)value->value.data)->data);
        has_time_c2 = 1;
        printf("C2 odebrano czas\n");
        time_received_c2 = time(NULL); 
        try_enqueue();
    }
}

static void handle_temp(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.temperature = *(UA_Double *)value->value.data;
        has_temp = 1;
        printf("C2 odebrano temp %f\n", currentData.temperature);
        try_enqueue();
    }
}

static void handle_wind(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.wind_speed = *(UA_Double *)value->value.data;
        has_wind = 1;
        printf("C2 odebrano wind %f\n", currentData.wind_speed);
        try_enqueue();
    }
}

static void handle_cloud(UA_Client *client, UA_UInt32 subId, void *subContext,
                         UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_INT32])) {
        currentData.solar_radiation = *(UA_Int32 *)value->value.data;
        has_cloud = 1;
        printf("C2 odebrano cloud %d\n", currentData.solar_radiation);
        try_enqueue();
    }
}

void *receive_c2_thread(void *arg) {
    const char *url = getenv("C2_SERVER_URL");
    if (!url) {
        fprintf(stderr, "[C2 OPCUA] Brak C2_SERVER_URL w .env\n");
        return NULL;
    }

    UA_Client *client_c2 = create_opcua_client(url);
    if (!client_c2) {
        fprintf(stderr, "[C2 OPCUA] Nie udalo sie polaczyc z serwerem\n");
        return NULL;
    }

    UA_CreateSubscriptionRequest req_c2 = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse resp_c2 = UA_Client_Subscriptions_create(client_c2, req_c2, NULL, NULL, NULL);

    if (resp_c2.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        UA_MonitoredItemCreateRequest mon1_c2 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 3002));
        UA_MonitoredItemCreateRequest mon2_c2 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 3003));
        UA_MonitoredItemCreateRequest mon3_c2 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 3004));
        UA_MonitoredItemCreateRequest mon4_c2 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 3005));

        UA_Client_MonitoredItems_createDataChange(client_c2, resp_c2.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon1_c2, NULL, handle_time_c2, NULL);
        UA_Client_MonitoredItems_createDataChange(client_c2, resp_c2.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon2_c2, NULL, handle_temp, NULL);
        UA_Client_MonitoredItems_createDataChange(client_c2, resp_c2.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon3_c2, NULL, handle_wind, NULL);
        UA_Client_MonitoredItems_createDataChange(client_c2, resp_c2.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon4_c2, NULL, handle_cloud, NULL);
    }

    while (1) {
        UA_Client_run_iterate(client_c2, 100);
        usleep(100000);
        if (time_received_c2 != 0 && time(NULL) - time_received_c2 >= 100 && has_time_c2) {
                wait_flag_c2 = 1;
                try_enqueue();
        }
    }

    UA_Client_delete(client_c2);
    return NULL;
}
