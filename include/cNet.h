#ifndef CNET_H
#define CNET_H
#include "client/client.h"
#include "server/server.h"
typedef enum CONFIGS_E {
    CONFIG_UNKNOWN=0,
    CONFIG_SERVER,
    CONFIG_CLIENT,
}CONFIGS_E;

struct cNet_control_t {
    CONFIGS_E cli_or_ser;
    struct cNet_serverConfig_t *server_config;
    struct cNet_clientConfig_t *client_config;
};
#endif