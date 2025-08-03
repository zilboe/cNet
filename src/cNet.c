#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cNet.h"
#include "memory/mempool.h"
#include "config/config.h"
int main(int argc, const char *argv[])
{
    static unsigned char get_config_flag = 0;
    char *config_file;
    struct cNet_control_t *cNet=NULL;
    if(argc != 3)
        return -1;
    cNet = (struct cNet_control_t*)cNet_malloc(sizeof(struct cNet_control_t));
    if(!cNet)
        return -1;

    for(int i=1; i<argc; i++) {
        if(strncmp(argv[i], "-s", strlen("-s")) == 0) {
            if(!get_config_flag) {
                cNet->cli_or_ser = CONFIG_SERVER;
                get_config_flag = 1;
            } else {
                return -1;
            }
        } else if(strncmp(argv[i], "-c", strlen("-c")) == 0) {
            if(!get_config_flag) {
                cNet->cli_or_ser = CONFIG_CLIENT;
                get_config_flag = 1;
            } else {
                return -1;
            }
        } else {
            config_file = (char*)argv[i];
        }
    }

    if(!config_file)
        return -1;
    
    if(cNet_parse_config(config_file, cNet) == -1) {
        printf("cNet Err: Config Parse Error\n");
        return -1;
    }

#if 1
    if(cNet->cli_or_ser == CONFIG_SERVER) {
        printf("Server Info: Server Port:[%d]\n", cNet->server_config->local_port);
    } else if(cNet->cli_or_ser == CONFIG_CLIENT) {
        printf("Client Info: Server IP:[%s]\n", cNet->client_config->server_ip);
        printf("Client Info: Server PORT:[%d]\n", cNet->client_config->server_port);
        while(cNet->client_config->config) {
            printf("Client Info: Config Head:[%s]\n", cNet->client_config->config->head);
            printf("Client Info: Config Name:[%s]\n", cNet->client_config->config->name);
            printf("Client Info: Config Type:[%s]\n", cNet->client_config->config->type);
            printf("Client Info: Config Localip:[%s]\n", cNet->client_config->config->local_ip);
            printf("Client Info: Config Localport:[%d]\n", cNet->client_config->config->local_port);
            printf("Client Info: Config Remoteport:[%d]\n", cNet->client_config->config->remote_port);
            cNet->client_config->config = cNet->client_config->config->next;
        }
    }
#endif
    return 0;
}