#include "config.h"
#include "server.h"
#include "client.h"
int cNet_strStripSpaceInPlace(char *content, int size)
{
    int write_pos=0;
    
    if (!content || size<=0)
        return 0;

    for (int read_pos=0; read_pos<size; read_pos++) {
        if (content[read_pos] != ' ') {
            content[write_pos++] = content[read_pos];
        }
    }

    content[write_pos] = '\0';
    
    return write_pos;
}

void cNet_strTolower(char *content, int size)
{
    int pos=0;
    if(!content || size==0)
        return;
    for(int i=0; i<size; i++) {
        content[pos++] = (content[i]>='A'&&content[i]<='Z') ? (content[i]+(unsigned char)32) : (content[i]);
    }
    content[pos] = '\0';
}

int cNet_strCmpTitle(char **content, int size, int line)
{
    char *pTitle=SERVER_TITLE_PORT;
    int i=0;
    if(**content != 'b') {
        printf("Server Err: Config File Line[%d] Unkown\n", line);
        return -1;
    }
    for(i=1; i<SERVER_TITLE_PORT_LEN&&i<size; i++) {
        if((*((*content)+i)) != *(pTitle+i)) {
            printf("Server Err: Config File Line[%d] Unkown,Do You Mean \"%s\"\n", line, SERVER_TITLE_PORT);
            return -1;
        }
    }
    if(i < SERVER_TITLE_PORT_LEN) {
        printf("Server Err: Config File Line[%d] Unkown,Do You Mean \"%s\"\n", line, SERVER_TITLE_PORT);
        return -1;
    }
    (*content) += SERVER_TITLE_PORT_LEN;
    return 0;
}

int cNet_parseServer(char *content, int size, cNetControl_t *pCnet, int line)
{
    int analy_size=0;
    char *p=content;
	if(size==0 || p==NULL) 
		goto parse_ser_with_err;
    analy_size = cNet_strStripSpaceInPlace(p, size);
    if(!analy_size)
        goto parse_ser_with_err;

    if(*p == '\n')
        return 0;

    *(p+size-1) = '\0';
    cNet_strTolower(p, size);

    if(cNet_strCmpTitle(&p, size, line) == 0) {

        if(*p++!='=')
            goto parse_ser_with_err;
        
        if(cNet_bindSerPort(pCnet, atoi(p)) == -1) 
            goto parse_ser_with_err;
    } else {
        return -1;
    }
    
    return 0;
parse_ser_with_err:
    return -1;

}

int cNet_parseClient(char *content, int size, cNetControl_t *pCnet, int line)
{
    char *p=content;
    int analy_size=0;
    if(size==0 || content==NULL) {
        printf("Client:Parse Config Err\n");
        return -1;
    }
    analy_size = cNet_strStripSpaceInPlace(p, size);
    if(!analy_size)
        goto parse_cli_with_err;
        
    cNet_strTolower(p, size);

    if(strncmp(p, CLIENT_TITLE_REMOTEADDR, CLIENT_TITLE_REMOTEADDR_LEN) == 0) {
        
    } else if(strncmp(p, CLIENT_TITLE_REMOTEPORT, CLIENT_TITLE_REMOTEPORT_LEN) == 0) {

    } else {

    }
    
    return 0;
parse_cli_with_err:
    return -1;
}

int cNet_parseConfig(FILE *fp, CONFIG_E ser_or_cli, cNetControl_t *pCnet)
{
    char buffer[256];
    int buff_len=0;
    int parse_result=0;
    int line=0;
    pCnet->cNet_config = ser_or_cli;
    memset(buffer, 0x0, sizeof(buffer));
    while(fgets(buffer, sizeof(buffer), fp)!=NULL) {
        buff_len = strlen(buffer);
        if(ser_or_cli == SERVER_CONFIG)
            parse_result = cNet_parseServer(buffer, buff_len, pCnet, ++line);
        else
            parse_result = cNet_parseClient(buffer, buff_len, pCnet, ++line);
        
        if(parse_result)
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


