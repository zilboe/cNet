#include "cNet.h"
#include "config.h"
int cNet_bindSerPort(cNetControl_t *pCnet, unsigned short port)
{
    static unsigned char cnet_get_port=0;
    if(cnet_get_port == 1)
        return 0;
    cnet_get_port = 1;
    if(port<0 || port>65535) {
        printf("Server:cNet Port Err\n");
        return -1;
    }
    pCnet->cNet_local_port = port;
    return 0;
}

void cNet_initArgument(cNetControl_t *pCnet)
{
    pCnet->cNet_local_port = 0;
    pCnet->cNet_local_port = 0;
    pCnet->cNet_remote_ip = 0;
    pCnet->cNet_remote_port = 0;
    pCnet->cNet_get_conf_flag = 0;
    pCnet->cNet_config = NOKOWN_CONFIG;
}

int main(int argc, const char *argv[])
{
    cNetControl_t cNet;
    cNet_initArgument(&cNet);

    if(cNet_parseArgv(argc, argv, &cNet) == -1) {
        return -1;
    }

    if(cNet.cNet_get_conf_flag == 0) {
        return -1;
    }

    

    return 0;
}
