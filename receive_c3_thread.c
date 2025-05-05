#include "receive_c3_thread.h"
#include "client_connection.h"
#include "fifo_queue.h"
#include "global.h"
#include "env_loader.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
static time_t time_received = 0;
static int wait_flag = 1;
static WeatherData currentData;
static int has_time = 0, has_cieplne = 0, has_wodne = 0, has_wiatrowe = 0, has_foto = 0;
static int has_inne = 0, has_cz = 0, has_de = 0, has_sk = 0, has_lt = 0;
static int has_ua = 0, has_se = 0, has_freq = 0;

static void try_enqueue() {
    if ((has_time && has_cieplne && has_wodne && has_wiatrowe 
        && has_cz && has_de && has_sk && has_lt && has_ua && has_freq) || wait_flag) {
        enqueue(currentData);
        //memset(&currentData, 0, sizeof(currentData));
        wait_flag = 0;
        has_time = has_cieplne = has_wodne = has_wiatrowe = has_foto = 0;
        has_inne = has_cz = has_de = has_sk = has_lt = has_ua = has_se = has_freq = 0;
    }
}

static void handle_time(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {
        strncpy(currentData.source, "C3", sizeof(currentData.source));
        snprintf(currentData.timestamp_c3, sizeof(currentData.timestamp_c3), "%.*s",
                 (int)((UA_String *)value->value.data)->length,
                 ((UA_String *)value->value.data)->data);
        has_time = 1;
        printf("C3 odebrano czas\n");
        time_received = time(NULL);
        try_enqueue();
    }
}

static void handle_cieplne(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.cieplne = *(UA_Double *)value->value.data;
        has_cieplne = 1;
        printf("C3 odebrano cieplne\n");
        try_enqueue();
    }
}

static void handle_wodne(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.wodne = *(UA_Double *)value->value.data;
        has_wodne = 1;
        printf("C3 odebrano wodne\n");
        try_enqueue();
    }
}

static void handle_wiatrowe(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.wiatrowe = *(UA_Double *)value->value.data;
        has_wiatrowe = 1;
        printf("C3 odebrano wiatrowe\n");
        try_enqueue();
    }
}
    
static void handle_fotowoltaiczne(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.fotowoltaiczne = *(UA_Double *)value->value.data;
        has_foto = 1;
        printf("C3 odrano foto\n");
        try_enqueue();
    }
}

static void handle_inne(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.inne = *(UA_Double *)value->value.data;
        has_inne = 1;
        printf("C3 odebrano inne\n");
        try_enqueue();
    }
}

static void handle_cz(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.cz = *(UA_Double *)value->value.data;
        has_cz = 1;
        printf("C3 odebrano cz\n");
        try_enqueue();
    }
}

static void handle_de(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.de = *(UA_Double *)value->value.data;
        has_de = 1;
        printf("C3 odebrano de\n");
        try_enqueue();
    }
}

static void handle_sk(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.sk = *(UA_Double *)value->value.data;
        has_sk = 1;
        printf("C3 odebrano sk\n");
        try_enqueue();
    }
}

static void handle_lt(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.lt = *(UA_Double *)value->value.data;
        has_lt = 1;
        printf("C3 odebrano lt\n");
        try_enqueue();
    }
}

static void handle_ua(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.ua = *(UA_Double *)value->value.data;
        has_ua = 1;
        printf("C3 odebrano ua\n");
        try_enqueue();
    }
}

static void handle_se(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.se = *(UA_Double *)value->value.data;
        has_se = 1;
        printf("C3 odebrano se\n");
        try_enqueue();
    }
}

static void handle_frequency(UA_Client *client, UA_UInt32 subId, void *subContext,
                        UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
        currentData.frequency = *(UA_Double *)value->value.data;
        has_freq = 1;
        printf("C3 odebrano freq\n");
        try_enqueue();
    }
}

void *receive_c3_thread(void *arg) {
    const char *url = getenv("C3_SERVER_URL");
    if (!url) {
        fprintf(stderr, "[C3 OPCUA] Brak C3_SERVER_URL w .env\n");
        return NULL;
    }

    UA_Client *client = create_opcua_client(url);
    if (!client) {
        fprintf(stderr, "[C3 OPCUA] Nie udalo sie polaczyc z serwerem\n");
        return NULL;
    }


    UA_CreateSubscriptionRequest req = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse resp = UA_Client_Subscriptions_create(client, req, NULL, NULL, NULL);

    if (resp.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        UA_MonitoredItemCreateRequest mon1 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4014));
        UA_MonitoredItemCreateRequest mon2 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4002));
        UA_MonitoredItemCreateRequest mon3 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4003));
        UA_MonitoredItemCreateRequest mon4 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4004));
        UA_MonitoredItemCreateRequest mon5 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4005));
        UA_MonitoredItemCreateRequest mon6 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4006));
        UA_MonitoredItemCreateRequest mon7 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4007));
        UA_MonitoredItemCreateRequest mon8 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4008));
        UA_MonitoredItemCreateRequest mon9 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4009));
        UA_MonitoredItemCreateRequest mon10 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4010));
        UA_MonitoredItemCreateRequest mon11 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4011));
        UA_MonitoredItemCreateRequest mon12 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4012));
        UA_MonitoredItemCreateRequest mon13 = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 4013));

        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon1, NULL, handle_time, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon2, NULL, handle_cieplne, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon3, NULL, handle_wodne, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon4, NULL, handle_wiatrowe, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon5, NULL, handle_fotowoltaiczne, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon6, NULL, handle_inne, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon7, NULL, handle_cz, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon8, NULL, handle_de, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon9, NULL, handle_sk, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon10, NULL, handle_lt, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon11, NULL, handle_ua, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon12, NULL, handle_se, NULL);
        UA_Client_MonitoredItems_createDataChange(client, resp.subscriptionId, UA_TIMESTAMPSTORETURN_SOURCE, mon13, NULL, handle_frequency, NULL);
    }

    while (1) {
        UA_Client_run_iterate(client, 100);
        usleep(100000);
        if (time_received != 0 && time(NULL) - time_received >= 100 && has_time) {
                wait_flag = 1;
                try_enqueue();
        }
    }

    UA_Client_delete(client);
    return NULL;
}
