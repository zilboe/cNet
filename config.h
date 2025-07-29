#ifndef CONFIG_H
#define	CONFIG_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cNet.h"

#define     SERVER_TITLE_1_CONTENT           ("bindport")
#define     SERVER_TITLE_1_LEN                (strlen(SERVER_TITLE_1_CONTENT))
extern int cNet_parseArgv(int argc, const char *argv[], cNetControl_t *pCnet);



#endif
