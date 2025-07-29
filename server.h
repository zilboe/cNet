#ifndef SERVER_H
#define SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#define	SSH_PORT	22
#define PORT		4051
#define BUFFER_SIZE	4096
#define	SSH_IP		"127.0.0.1"
#endif