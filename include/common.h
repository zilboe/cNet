#ifndef COMMON_H
#define COMMON_H

struct cNet_config_t {
    char *key;
    char *value;
    struct cNet_config_t *next;
};

struct cNet_tcpNode_t {
	int tcp_fd;
	unsigned int ip;
	unsigned short port;
	struct cNet_tcpNode_t *next;
};

struct cNet_tcpList_t {
	struct cNet_tcpNode_t *head;
	int size;
};

#endif
