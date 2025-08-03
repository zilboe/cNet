#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cNet.h"
#include "memory/mempool.h"
#include "config/config.h"
#if defined(TEST)
#include "test.h"
#endif

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
    
    if(cNet_parse_config(config_file, cNet) == -1)
        return -1;

    #if defined(TEST)
        cNet_print();
    #endif


    return 0;
}

