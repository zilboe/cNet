#include "config.h"

int cNet_stripSpaceInPlace(char *content, int size)
{
    int write_pos=0;
    
    if (!content || size<=0)
        return 0;

    for (int read_pos = 0; read_pos < size; read_pos++) {
        if (content[read_pos] != ' ') {
            content[write_pos++] = content[read_pos];
        }
    }

    content[write_pos] = '\0';
    
    return write_pos;
}

int cNet_parseServer(char *content, int size, cNetControl_t *pCnet)
{
    int port=0;
    int analy_size=0;
    char *p=content;
	if(size==0 || p==NULL) 
		goto parse_ser_with_err;
    
    if(*p == '\n'){
        return 0;
    }

    *(p+size-1) = '\0';

    analy_size = cNet_stripSpaceInPlace(p, size);

    if(!analy_size)
        goto parse_ser_with_err;

    if(strncmp(p, SERVER_TITLE_1_CONTENT, SERVER_TITLE_1_LEN) == 0) {
        p += SERVER_TITLE_1_LEN;
        if(*p++!='=')
            goto parse_ser_with_err;
        
        port = atoi(p);
        if(cNet_bindSerPort(pCnet, port) == -1) 
            goto parse_ser_with_err;
    }

    return 0;
parse_ser_with_err:
    return -1;

}

int cNet_parseClient(char *content, int size, cNetControl_t *pCnet)
{
    if(size == 0) {
        printf("Client:Parse Config Err\n");
        return -1;
    }

    return 0;
}

int cNet_parseConfig(FILE *fp, CONFIG_E ser_or_cli, cNetControl_t *pCnet)
{
    char buffer[256];
    int buff_len=0;
    int parse_result=0;
    pCnet->cNet_config = ser_or_cli;
    memset(buffer, 0x0, sizeof(buffer));
    while(fgets(buffer, sizeof(buffer), fp)!=NULL) {
        buff_len = strlen(buffer);
        if(ser_or_cli == SERVER_CONFIG)
            parse_result = cNet_parseServer(buffer, buff_len, pCnet);
        else
            parse_result = cNet_parseClient(buffer, buff_len, pCnet);
        
        if(!parse_result)
            break;
    }
    return 0;
}

int cNet_parseArgv(int argc, const char *argv[], cNetControl_t *pCnet)
{
    char *srouce_config=NULL;
    CONFIG_E server_or_client=NOKOWN_CONFIG;
    if(argc == 0) {
        printf("Config Is Null\n");
        return -1;
    }
    
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-c")==0) {
            server_or_client = CLIENT_CONFIG;
        } else if(strcmp(argv[i], "-s")==0) {
            server_or_client = SERVER_CONFIG;
        } else {
            if(server_or_client!=SERVER_CONFIG && server_or_client!=CLIENT_CONFIG) {
                srouce_config = NULL;
            } else {
                srouce_config = (char*)argv[i];
            }
        }
    }
    FILE *fp=fopen(srouce_config, "r");
    if(fp == NULL) {
        printf("Config noKown\n");
        return -1;
    }

    if(cNet_parseConfig(fp, server_or_client, pCnet) == -1) {
        printf("Config Err\n");
        return -1;
    }

    return 0;
}


