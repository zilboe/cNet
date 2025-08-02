#ifndef CLIENT_H
#define CLIENT_H
#include "common.h"

struct cNet_mapConfig_t {
    char *head;
    char *name;
    char *type;
    char *local_ip;
    unsigned short local_port;
    unsigned short server_port;
    struct cNet_config_t *config;
    struct cNet_mapConfig_t *next;
};

struct cNet_clientConfig_t {
    char *server_ip;
    unsigned short server_port;
    int config_size;
    struct cNet_mapConfig_t *config; 
};
#endif
