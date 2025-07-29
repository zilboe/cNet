#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT	22
#define	BUFFER_SIZE	4096
int main(int argc, const char *argv[])
{
	int client_fd;
	struct sockaddr_in server_sock;
	char buffer[BUFFER_SIZE];
	memset(&server_sock, 0, sizeof(server_sock));
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd < 0) {
		printf("Client Socket Create Error\n");
		return -1;
	}
	server_sock.sin_family = AF_INET;
	server_sock.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &server_sock.sin_addr);
	if(connect(client_fd, (struct sockaddr *)&server_sock, sizeof(server_sock)) == -1) {
		printf("Client Error:Connect Failed\n");
		return -1;
	}
	printf("Client Message:Connect Success\n");
	while(true) {
		printf("Client Message:");
		memset(buffer, 0x0, sizeof(buffer));
		scanf("%s", buffer);
		int len = strlen(buffer);
		if(send(client_fd, buffer, len, 0) == -1) {
			printf("Client Message:Send Failed\n");
			break;
		}
	}
	close(client_fd);

	return 0;
}
