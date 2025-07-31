#ifndef CONFIG_H
#define	CONFIG_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cNet.h"

typedef enum CNET_STRING_ERR {
    CNET_SUCCESS = 0,
    CNET_MISMATCH_ERR,
    CNET_KEY_ERR,
    CNET_VALUE_ERR,
    CNET_OTHER_CH_ERR,
    CNET_UNMEM_ERR,
    CNET_UNKOWN_ERR,
}CNET_STRING_ERR;


typedef struct cNet_title_t {
    char *title;
    int title_len;
    CNET_STRING_ERR(*cNet_titleCallback)(cNetControl_t *pCnet, char *key, char *value, int line);
}cNet_title_t;

extern int cNet_parseArgv(int argc, const char *argv[], cNetControl_t *pCnet);


#endif
