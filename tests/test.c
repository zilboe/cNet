#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"
void cNet_print(struct cNet_control_t *p)
{
    if(p->cli_or_ser == CONFIG_SERVER) {
        printf("Server Info: Server Port:[%d]\n", p->server_config->local_port);
    } else if(p->cli_or_ser == CONFIG_CLIENT) {
        printf("Client Info: Server IP:[%s]\n", p->client_config->server_ip);
        printf("Client Info: Server PORT:[%d]\n", p->client_config->server_port);
        while(p->client_config->config) {
            printf("Client Info: Config Head:[%s]\n", p->client_config->config->head);
            printf("Client Info: Config Name:[%s]\n", p->client_config->config->name);
            printf("Client Info: Config Type:[%s]\n", p->client_config->config->type);
            printf("Client Info: Config Localip:[%s]\n", p->client_config->config->local_ip);
            printf("Client Info: Config Localport:[%d]\n", p->client_config->config->local_port);
            printf("Client Info: Config Remoteport:[%d]\n", p->client_config->config->remote_port);
            p->client_config->config = p->client_config->config->next;
        }
    }
}