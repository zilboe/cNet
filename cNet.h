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

typedef struct cNet_cliControl_t {
    unsigned int cNet_local_ip;
    unsigned int cNet_remote_ip;
    unsigned short cNet_local_port;
    unsigned short cNet_remote_port;
    unsigned short cNet_init;
    struct cNet_cliControl_t *next;
}cNet_cliControl_t;

typedef struct cNet_serControl_t {
    unsigned short cNet_local_port;
    unsigned short cNet_init;
}cNet_serControl_t;

typedef int (*cNet_initCall)(struct cNetControl_t *);
typedef struct cNetControl_t {
    CONFIG_E    cNet_config;
    cNet_cliControl_t *cNet_client;
    cNet_serControl_t *cNet_server;
    cNet_initCall init;
} cNetControl_t;

int cNet_serBindport(cNetControl_t *pCnet, unsigned short port);

#endif
