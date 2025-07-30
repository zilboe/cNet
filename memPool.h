#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *cNet_malloc(int size);
void cNet_free(void *p);
#endif