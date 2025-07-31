#ifndef CNET_H
#define	CNET_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum CONFIG_E {
    NOKOWN_CONFIG,
    SERVER_CONFIG,
    CLIENT_CONFIG,
}CONFIG_E;

typedef struct cNet_cliMap_t {
    bool cNet_map_config_flag;
    char *cNet_map_name;
    char *cNet_map_type;
    char *cNet_map_local_ip;
    unsigned short cNet_map_local_port;
    char *cNet_map_server_ip;
    unsigned short cNet_map_server_port;
    struct cNet_cliMap_t *next;
}cNet_cliMap_t;

typedef struct cNet_cliControl_t {
    char *cNet_server_ip;
    unsigned short cNet_server_port;
    bool init_flag;
    int cNet_map_size;
    cNet_cliMap_t *cNet_map_config;
}cNet_cliControl_t;

typedef struct cNet_serControl_t {
    unsigned short cNet_local_port;
    bool init_flag;
}cNet_serControl_t;


typedef struct cNetControl_t {
    CONFIG_E    cNet_config;
    cNet_cliControl_t *cNet_client;
    cNet_serControl_t *cNet_server;
    int (*cNet_init)(struct cNetControl_t);
} cNetControl_t;


#endif
