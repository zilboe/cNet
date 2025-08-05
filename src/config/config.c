#include "cNet.h"
#include "config/config.h"
#include "memory/mempool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define     TITLE_BINDPORT          ("bindport")
#define     TITLE_BINDPORT_LEN      (strlen(TITLE_BINDPORT))
#define     TITLE_SERVERADDR        ("serveraddr")
#define     TITLE_SERVERADDR_LEN    (strlen(TITLE_SERVERADDR))
#define     TITLE_SERVERPORT        ("serverport")
#define     TITLE_SERVERPORT_LEN    (strlen(TITLE_SERVERPORT))
#define     TITLE_PROXIES           ("proxies")
#define     TITLE_PROXIES_LEN       (strlen(TITLE_PROXIES))
#define     TITLE_NAME              ("name")
#define     TITLE_NAME_LEN          (strlen(TITLE_NAME))
#define     TITLE_TYPE              ("type")
#define     TITLE_TYPE_LEN          (strlen(TITLE_TYPE))
#define     TITLE_LOCALIP           ("localip")
#define     TITLE_LOCALIP_LEN       (strlen(TITLE_LOCALIP))
#define     TITLE_LOCALPORT         ("localport")
#define     TITLE_LOCALPORT_LEN     (strlen(TITLE_LOCALPORT))
#define     TITLE_REMOTEPORT        ("remoteport")
#define     TITLE_REMOTEPORT_LEN    (strlen(TITLE_REMOTEPORT))

typedef struct cNet_title_t {
    char *title;
    ssize_t title_len;
    int (*parse_callback)(const char *key, const char *value, struct cNet_control_t *p);
}cNet_title_t;


static int cNet_config_bindport(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_serveraddr(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_serverport(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_proxies(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_name(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_type(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_localip(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_localport(const char *key, const char *value, struct cNet_control_t *p);
static int cNet_config_remoteport(const char *key, const char *value, struct cNet_control_t *p);

static cNet_title_t cNet_Head[] = {
    {TITLE_BINDPORT, TITLE_BINDPORT_LEN, &cNet_config_bindport},
    {TITLE_SERVERADDR, TITLE_SERVERADDR_LEN, &cNet_config_serveraddr},
    {TITLE_SERVERPORT, TITLE_SERVERPORT_LEN, &cNet_config_serverport},
    {TITLE_PROXIES, TITLE_PROXIES_LEN, &cNet_config_proxies},
    {TITLE_NAME, TITLE_NAME_LEN, &cNet_config_name},
    {TITLE_TYPE, TITLE_TYPE_LEN, &cNet_config_type},
    {TITLE_LOCALIP, TITLE_LOCALIP_LEN, &cNet_config_localip},
    {TITLE_LOCALPORT, TITLE_LOCALPORT_LEN, &cNet_config_localport},
    {TITLE_REMOTEPORT, TITLE_REMOTEPORT_LEN, &cNet_config_remoteport}
};

static int cNet_config_bindport(const char *key, const char *value, struct cNet_control_t *p)
{
    int port=0;
    if(!value)
        return -1;
    if(p->cli_or_ser != CONFIG_SERVER)
        return -1;
    if(!p->server_config)
        return -1;
    port = atoi(value);
    if(port<0 || port>65535)
        return -1;
    p->server_config->local_port = (unsigned short)port;
    return 0;
}

static int cNet_config_serveraddr(const char *key, const char *value, struct cNet_control_t *p)
{
    int value_len=0;
    if(!value)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)
        return -1;
    value_len = strlen(value);
    if(!value_len)
        return -1;
    p->client_config->server_ip = (char*)cNet_malloc(value_len);
    if(!p->client_config->server_ip)
        return -1;
    strncpy(p->client_config->server_ip, value, value_len);
    return 0;
}

static int cNet_config_serverport(const char *key, const char *value, struct cNet_control_t *p)
{
    int port=0;
    if(!value)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)
        return -1;
    port = atoi(value);
    if(port<0 || port>65535)
        return -1;
    p->client_config->server_port = (unsigned short)port;
    return 0;
}

static int cNet_config_proxies(const char *key, const char *value, struct cNet_control_t *p)
{
    int key_len=0;
    struct cNet_mapConfig_t *p_config=NULL;
    struct cNet_mapConfig_t *p_new=NULL;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)   
        return -1;
    if(!key)
        return -1;
    key_len = strlen(key);
    p_new = (struct cNet_mapConfig_t*)cNet_malloc(sizeof(struct cNet_mapConfig_t));
    if(!p_new)
        return -1;

    p_new->head = (char*)cNet_malloc(sizeof(key_len));
    if(!p_new->head)
        return -1;

    if(!p->client_config->config) {
        p->client_config->config = p_new;
        p->client_config->config_size = 1;
        strncpy(p_new->head, key, key_len);
        return 0;
    }

    p_config = p->client_config->config;

    while(p_config->next)
        p_config = p_config->next;
    p_config->next = p_new;
    p->client_config->config_size += 1;
    strncpy(p_new->head, key, key_len);
    return 0;
}

static int cNet_config_name(const char *key, const char *value, struct cNet_control_t *p)
{
    struct cNet_mapConfig_t *p_config=NULL;
    int value_len=0;
    if(!value || !p)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)   
        return -1;
    if(!p->client_config->config)
        return -1;
    p_config = p->client_config->config;
    while(p_config->next)
        p_config = p_config->next;
    value_len = strlen(value);
    p_config->name = (char*)cNet_malloc(sizeof(value_len));
    if(!p_config->name)
        return -1;
    strncpy(p_config->name, value, value_len);
    return 0;
}

static int cNet_config_type(const char *key, const char *value, struct cNet_control_t *p)
{
    struct cNet_mapConfig_t *p_config=NULL;
    int value_len=0;
    if(!value || !p)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)   
        return -1;
    if(!p->client_config->config)
        return -1;
    p_config = p->client_config->config;
    while(p_config->next)
        p_config = p_config->next;
    value_len = strlen(value);
    p_config->type = (char*)cNet_malloc(sizeof(value_len));
    if(!p_config->type)
        return -1;
    strncpy(p_config->type, value, value_len);
    return 0;
}

static int cNet_config_localip(const char *key, const char *value, struct cNet_control_t *p)
{
    struct cNet_mapConfig_t *p_config=NULL;
    int value_len=0;
    if(!value || !p)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)   
        return -1;
    if(!p->client_config->config)
        return -1;
    p_config = p->client_config->config;
    while(p_config->next)
        p_config = p_config->next;
    value_len = strlen(value);
    p_config->local_ip = (char*)cNet_malloc(sizeof(value_len));
    if(!p_config->local_ip)
        return -1;
    strncpy(p_config->local_ip, value, value_len);
    return 0;
}

static int cNet_config_localport(const char *key, const char *value, struct cNet_control_t *p)
{
    struct cNet_mapConfig_t *p_config=NULL;
    int port=0;
    if(!value || !p)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)   
        return -1;
    if(!p->client_config->config)
        return -1;
    p_config = p->client_config->config;
    while(p_config->next)
        p_config = p_config->next;
    
    port = atoi(value);
    if(port<0 || port>65535)
        return -1;
    p_config->local_port = port;
    return 0;
}

static int cNet_config_remoteport(const char *key, const char *value, struct cNet_control_t *p)
{
    struct cNet_mapConfig_t *p_config=NULL;
    int port=0;
    if(!value || !p)
        return -1;
    if(p->cli_or_ser != CONFIG_CLIENT)
        return -1;
    if(!p->client_config)   
        return -1;
    if(!p->client_config->config)
        return -1;
    p_config = p->client_config->config;
    while(p_config->next)
        p_config = p_config->next;
    
    port = atoi(value);
    if(port<0 || port>65535)
        return -1;
    p_config->remote_port = port;
    return 0;
}


void cNet_to_lower(char *line, int size)
{
    int write_pos=0;
    if(!line || size==0)
        return;
    for(int i=0; i<size; i++) {
        line[write_pos++] = (line[i]>='A'&&line[i]<='Z') ? (line[i]+(char)32) : (line[i]);
    }
}



int cNet_strip_other_char(char *line, int size)
{
    int write_pos=0;
    if(!line || size==0)
        return 0;
    for(int i=0; i<size; i++) {
        if(line[i]!=' ' && line[i]!='"' && line[i]!='\n' && line[i]!='[' && line[i]!=']')
            line[write_pos++] = line[i];
    }
    line[write_pos] = '\0';

    return write_pos;
}



int cNet_parse_line(char *line, int size, struct cNet_control_t *p)
{
    int len=0;
    char *key=NULL;
    char *value=NULL;
    int value_err=0;
    if(!p || !line || size==0)
        return -1;

    cNet_to_lower(line, size);
    len = cNet_strip_other_char(line, size);
    if(!len)
        return 0;
    key = strtok(line, "=");
    value = strtok(NULL, "=");
    len = 0;
    len = strlen(key);
    for(int i=0; i<(sizeof(cNet_Head)/sizeof(cNet_Head[0])); i++) {
        if(len != cNet_Head[i].title_len)
            continue;
        if(strncmp(key, cNet_Head[i].title, len) == 0) {
            value_err = cNet_Head[i].parse_callback(key, value, p);
            if(!value_err)
                return 0;
        }
    }

    return -1;
}

int cNet_parse_config(const char *file_name, struct cNet_control_t *p)
{
    int file_line=0;
    char buff[256];
    int buff_len=0;
    int parse_result=0;
    FILE *fp=NULL;
    if(!file_name || !p)
        return -1;
    fp = fopen(file_name, "r");
    if(!fp) {
        printf("cNet Err: The config file does not exist\n");
        return -1;
    }

    if(p->cli_or_ser == CONFIG_SERVER) {
        if(!p->server_config) {
            p->server_config = (struct cNet_serverConfig_t*)cNet_malloc(sizeof(struct cNet_serverConfig_t));
            p->run = cNet_runServer;
        }
        if(!p->server_config)
            return -1; 
    } else if(p->cli_or_ser == CONFIG_CLIENT) {
        if(!p->client_config) {
            p->client_config = (struct cNet_clientConfig_t*)cNet_malloc(sizeof(struct cNet_clientConfig_t));
            p->run = cNet_runClient;
        }
        if(!p->client_config)
            return -1;
    }
    if(!p->run)
        return -1;
    memset(buff, 0x0, sizeof(buff));
    while(fgets(buff, sizeof(buff)-1, fp) != NULL) {
        ++file_line;
        buff_len = strlen(buff);
        parse_result = cNet_parse_line(buff, buff_len, p);
        switch(parse_result) {
            case -1:
                printf("cNet Err: Config Parse Error,line[%d]\n", file_line);
                return -1;
            default:
                break;
        }
        memset(buff, 0x0, sizeof(buff));
        buff_len = 0;
    }
    return 0;
}