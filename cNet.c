#include "cNet.h"
#include "config.h"
int cNet_serBindport(cNetControl_t *pCnet, unsigned short port)
{
    static unsigned char cnet_get_port=0;
    if(cnet_get_port == 1)
        return 0;
    cnet_get_port = 1;
    if(port<0 || port>65535) {
        printf("Server:cNet Port Err\n");
        return -1;
    }
   
    return 0;
}


int main(int argc, const char *argv[])
{
    /*  */
    cNetControl_t cNet;
    memset(&cNet, 0x0, sizeof(cNetControl_t));

    /* Initialize the parameters and analyze the configuration file */
    if(cNet_parseArgv(argc, argv, &cNet) == -1) {
        return -1;
    }

    return 0;
}
