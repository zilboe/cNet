// #include <sys/types.h>
// #include <unistd.h>
#include "cNet.h"
#include <stdio.h>
int cNet_runServer(struct cNet_control_t *p)
{
    if(!p)
        return -1;
    printf("cNet Server: Running\n");
    return 0;
}
