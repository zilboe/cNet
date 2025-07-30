#ifndef CONFIG_H
#define	CONFIG_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cNet.h"
#include "client.h"
typedef int (*cNet_titleCallback)(cNetControl_t *pCnet, int len, int line);
typedef struct cNet_title_t {

    char *title;
    int title_len;
    cNet_titleCallback cNet_analyTitle; 
}cNet_title_t;

extern int cNet_parseArgv(int argc, const char *argv[], cNetControl_t *pCnet);



#endif
