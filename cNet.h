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

typedef struct cNetControl_t {
    CONFIG_E    cNet_config;

    unsigned short cNet_local_port;

    unsigned short cNet_remote_port;

    unsigned int cNet_local_ip;

    unsigned int cNet_remote_ip;
} cNetControl_t;

extern int cNet_bindSerPort(cNetControl_t *pCnet, unsigned short port);

#endif
