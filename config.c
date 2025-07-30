#include "config.h"
#include "server.h"
#include "client.h"
#define     SIZE(X)     (sizeof(X)/sizeof(X[0]))
static cNet_title_t cNet_confCliTitle[] = {
    {CLIENT_TITLE_SERVERADDR, CLIENT_TITLE_SERVERADDR_LEN, NULL},
    {CLIENT_TITLE_SERVERPORT, CLIENT_TITLE_SERVERPORT_LEN, NULL},
    {CLIENT_TITLE_PROXY, CLIENT_TITLE_PROXY_LEN, NULL},
    {CLIENT_TITLE_NAME, CLIENT_TITLE_NAME_LEN, NULL},
    {CLIENT_TITLE_TYPE, CLIENT_TITLE_TYPE_LEN, NULL},
    {CLIENT_TITLE_LOCALIP, CLIENT_TITLE_LOCALIP_LEN, NULL},
    {CLIENT_TITLE_LOCALPORT, CLIENT_TITLE_LOCALPORT_LEN, NULL},
    {CLIENT_TITLE_REMOTEPORT, CLIENT_TITLE_REMOTEPORT_LEN, NULL},
};

static cNet_title_t cNet_confSerTitle[] = {
    {SERVER_TITLE_PORT, SERVER_TITLE_PORT_LEN, NULL},
};

int cNet_strStripOtherCh(char *content, int size)
{
    int write_pos=0;
    
    if (!content || size<=0)
        return 0;

    for (int read_pos=0; read_pos<size; read_pos++) {
        if (content[read_pos]==' ' || content[read_pos]=='[' || content[read_pos]==']') 
            continue;
        content[write_pos++] = content[read_pos];
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

int cNet_strCmpErr(const char *src_content, int src_len, int line, cNet_title_t **pTitle, int title_size)
{
    int str_cmp_count[title_size];
    int max_len=0;
    int max_index=0;
    for(int i=0; i<title_size; i++)
        str_cmp_count[i] = 0;

    for(int i=0; i<title_size; i++) {
        for(int j=0; j<pTitle[i]->title_len&&j<src_len; j++) {
            if(src_content[j] == pTitle[i]->title[j])
                str_cmp_count[i]++;
        }
    }
    max_len=str_cmp_count[0];
    for(int i=0; i<title_size; i++) {
        if(max_len < str_cmp_count[i]) {
            max_len = str_cmp_count[i];
            max_index = i;
        }
    }
    printf("Server ERR: Config File Line[%d],Do you mean \"%s\"\n", line, pTitle[max_index]->title);
    return 0;
}

void cNet_strBreakDown(char *content, int size, char **key, char **value)
{
    int equal=0;
    *key = content;
    for(equal=0; equal<size; equal++) {
        if(content[equal]=='=') {
            content[equal] = '\0';
            *value = content[equal+1];
            return;
        }
    }
}

int cNet_strCmpTitle(char *content, int size, int line, cNetControl_t *pCnet)
{
    int title_size=0;
    int index=0;
    cNet_title_t *pDst_title=NULL;
    char *key=NULL;
    char *value=NULL;
    int i=0,j=0;

    title_size = (pCnet->cNet_config==SERVER_CONFIG) ? SIZE(cNet_confSerTitle) : SIZE(cNet_confCliTitle);
    pDst_title = (pCnet->cNet_config==SERVER_CONFIG) ? (&cNet_confSerTitle[0]) : (&cNet_confCliTitle[0]);

    /* I think it's necessary to break down the original content here for easier analysis */
    cNet_strBreakDown(content, size, &key, &value);

    for(i=0; i<title_size; i++) {
        
    }
    
    return 0;
}

int cNet_parseLine(char *content, int size, cNetControl_t *pCnet, int line)
{
    int analy_size=0;
    char *p=NULL;

    p = content;
	if(size==0 || p==NULL) 
		goto parse_with_err;

    /* Remove whitespace characters and other useless characters */
    analy_size = cNet_strStripOtherCh(p, size);
    if(!analy_size)
        goto parse_with_err;

    if(*p == '\n')
        return 0;

    *(p+size-1) = '\0';
    cNet_strTolower(p, size);

    if(cNet_strCmpTitle(p, size, line, pCnet) == 0) {

    } else {
        return -1;
    }
    
    return 0;
parse_with_err:
    return -1;

}

int cNet_parseConfig(FILE *fp, cNetControl_t *pCnet)
{
    char buffer[256];
    int buff_len=0;
    int parse_result=0;
    int line=0;
    memset(buffer, 0x0, sizeof(buffer));
    while(fgets(buffer, sizeof(buffer), fp)!=NULL) {
        buff_len = strlen(buffer);
        parse_result = cNet_parseLine(buffer, buff_len, pCnet, ++line);
        memset(buffer, 0x0, sizeof(buffer));
        if(parse_result)
            break;
    }
    return 0;
}

int cNet_parseArgv(int argc, const char *argv[], cNetControl_t *pCnet)
{
    char *p_config_name=NULL;
    FILE *fp=NULL;

    if(argc == 0) {
        printf("Config Is Null\n");
        return -1;
    }
    
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-c")==0) {
            pCnet->cNet_config = CLIENT_CONFIG;
        } else if(strcmp(argv[i], "-s")==0) {
            pCnet->cNet_config = SERVER_CONFIG;
        } else {
            if(pCnet->cNet_config!=SERVER_CONFIG && pCnet->cNet_config!=CLIENT_CONFIG) {
                p_config_name = NULL;
            } else {
                p_config_name = (char*)argv[i];
            }
        }
    }

    fp = fopen(p_config_name, "r");
    if(fp == NULL) {
        printf("cNet Err:There no config file\n");
        return -1;
    }

    if(cNet_parseConfig(fp, pCnet) == -1) {
        return -1;
    }

    return 0;
}


