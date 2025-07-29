all:
	gcc client.c -o client
	gcc server.c -o server
conf:
	gcc config.c cNet.c -o cNet
clean:
	rm client server cNet
