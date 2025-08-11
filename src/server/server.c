#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "cNet.h"
#include "thread/thread.h"
#define    MAX_EVENTS    1024
#define	   RECV_BUFFER_SIZE    4096
#define    SERVER_LISTEN_SIZE	10

int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) 
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int cNet_serverCreateSock(void)
{
    int sock=0;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("cNet Server:Socket Create Error\n");
	return -1;
    }
    int opt = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(&opt)) == -1) {
    	printf("cNet Server:Set Socket Error\n");
	close(sock);
	return -1;
    }
    return sock;
}

int cNet_runServer(struct cNet_control_t *p)
{
    int server_fd=0,epoll_fd=0;
    int ntfd;
    struct sockaddr_in server_addr;
    struct epoll_event ev, events[MAX_EVENTS];
    if(!p || !p->server_config) {
        printf("cNet Server: Can't Handle Server\n");
        return -1;
    }
    
    CThread_pool_t *thread_pool = ThreadPoolConstructDefault();
    if(!thread_pool) {
        return -2;
    }

    printf("cNet Server: Running\n");
    server_fd = cNet_serverCreateSock();
    if(server_fd == -1) {
    	return -1;
    }

    memset(&server_addr, 0x0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(p->server_config->local_port);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("cNet Server:Bind Server Error\n");
        close(server_fd);
        return -1;
    }

    if(listen(server_fd, SERVER_LISTEN_SIZE) == -1) {
        printf("cNet Server:Listen Server Error\n");
	    return -1;
    }

    epoll_fd = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    while(1) {
        ntfd = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if(ntfd < 0)
            continue;
        for(int i=0; i<ntfd; i++) {
            if(events[i].data.fd == server_fd) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if(client_fd == -1) {
                    printf("cNet Server:Client Accept Error\n");
                    continue;
                }
                if(set_nonblocking(client_fd) == -1) {
                    close(client_fd);
                    continue;
                }

                
            }
        }
    }

    close(server_fd);

    return 0;

}


