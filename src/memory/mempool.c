
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory/mempool.h"

void *cNet_malloc(int size)
{
    void *p=NULL;
    p = (void*)malloc(size);
    if(!p)
        return NULL;
    return p;
}

void cNet_free(void *p)
{
    if(!p)
        return;
    free(p);
}
