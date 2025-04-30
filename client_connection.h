#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <open62541/client.h>
#include <open62541/client_config_default.h>

//extern UA_Client *global_opcua_client;

UA_Client *create_opcua_client(const char *url);


#endif
