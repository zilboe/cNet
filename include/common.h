#ifndef COMMON_H
#define COMMON_H

struct cNet_config_t {
    char *key;
    char *value;
    struct cNet_config_t *next;
};

#endif
