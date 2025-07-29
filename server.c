#include "server.h"

typedef struct tcp_tunnel_t {
	int pthread_init;
	int src;
	int dst;
	char src_buffer[BUFFER_SIZE];
	char dst_buffer[BUFFER_SIZE];
	pthread_t src_to_dst_pthread;
	pthread_t dst_to_src_pthread;
}tcp_tunnel_t;

void Client_To_SSH(void *arg)
{
	int recv_lens;
	tcp_tunnel_t *clientSSH = (tcp_tunnel_t *)arg;
	if(clientSSH == NULL){
		printf("client ssh is null\n");
		goto src_to_dst_end;
	}
	if(clientSSH->dst == -1 || clientSSH->src == -1){
		printf("Tunnel Tcp Disconnect\n");
		goto src_to_dst_end;
	}

	while(true){
		memset(clientSSH->src_buffer, 0x0, BUFFER_SIZE);
		recv_lens = read(clientSSH->src, clientSSH->src_buffer, BUFFER_SIZE);
		if(recv_lens == -1 || recv_lens == 0){
			printf("Recv Client Error\n");
			break;
		}
		recv_lens = write(clientSSH->dst, clientSSH->src_buffer, recv_lens);
		if(recv_lens == -1 || recv_lens == 0) {
			printf("Send SSH Server Error\n");
			break;
		}
	}

src_to_dst_end:
	clientSSH->pthread_init = 0;
	pthread_detach(clientSSH->src_to_dst_pthread);
}

void SSH_To_Client(void *arg)
{
	int recv_lens;
	tcp_tunnel_t *clientSSH = (tcp_tunnel_t *)arg;
	if(clientSSH == NULL){
		printf("client ssh is null\n");
		goto src_to_dst_end;
	}
	if(clientSSH->dst == -1 || clientSSH->src == -1){
		printf("Tunnel Tcp Disconnect\n");
		goto src_to_dst_end;
	}

	while(true){
		memset(clientSSH->dst_buffer, 0x0, BUFFER_SIZE);
		recv_lens = read(clientSSH->dst, clientSSH->dst_buffer, BUFFER_SIZE);
		if(recv_lens == -1 || recv_lens == 0){
			printf("Recv SSH Error\n");
			break;
		}
		recv_lens = write(clientSSH->src, clientSSH->dst_buffer, recv_lens);
		if(recv_lens == -1 || recv_lens == 0) {
			printf("Send Client Error\n");
			break;
		}
	}

src_to_dst_end:
	clientSSH->pthread_init = 0;
	pthread_detach(clientSSH->dst_to_src_pthread);
}

int main(int argc, const char *argv[])
{
	/* Server Create Socket,Bind,Listen */
	int serverfd;
	int rets;
	int recv_lens;
	int send_lens;
	struct sockaddr_in socketaddr;
	memset(&socketaddr, 0, sizeof(struct sockaddr_in));
	socketaddr.sin_port = htons(PORT);
	socketaddr.sin_family = AF_INET;
	socketaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	rets = bind(serverfd, (struct sockaddr*)&socketaddr, sizeof(socketaddr));
	if(rets == -1) {
		printf("Server ERR:The Server Port[%d] Used\n", PORT);
		return -1;
	}
	rets = listen(serverfd, 5);
	if(rets == -1) {
		printf("Server ERR:The Server Port Listen Failed\n");
		return -1;
	}
	int client_fd = accept(serverfd, NULL, NULL);
	printf("Server Message:Client[%d] Connect\n", client_fd);

	int ssh_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in ssh_sockaddr;
	memset(&ssh_sockaddr, 0x0, sizeof(ssh_sockaddr));
	ssh_sockaddr.sin_family = AF_INET;
	ssh_sockaddr.sin_port = htons(SSH_PORT);
	inet_pton(AF_INET, SSH_IP, &ssh_sockaddr.sin_addr);
	connect(ssh_fd, (struct sockaddr *)&ssh_sockaddr, sizeof(ssh_sockaddr));

	tcp_tunnel_t clientSSH;
	clientSSH.src = client_fd;
	clientSSH.dst = ssh_fd;
	clientSSH.pthread_init = 1;
	pthread_create(&clientSSH.src_to_dst_pthread, NULL, (void*)&Client_To_SSH, (void*)&clientSSH);
	pthread_create(&clientSSH.dst_to_src_pthread, NULL, (void*)&SSH_To_Client, (void*)&clientSSH);
	while(true){
		if(!clientSSH.pthread_init){
			break;
		}
	}
	
	printf("Pthread End\n");
	return 0;
}
