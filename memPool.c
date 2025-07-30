#include "memPool.h"

void *cNet_malloc(int size)
{
    void *cNet_get_mem = (void*)malloc(size);
    if(!cNet_get_mem)
        return NULL;
    memset(cNet_get_mem, 0x0, size);
    return cNet_get_mem;
}

void cNet_free(void *p)
{
    if(!p)
        return;
    free(p);
}