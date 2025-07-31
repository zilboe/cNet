#include "cNet.h"
#include "config.h"

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


