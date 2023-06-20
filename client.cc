#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <cerrno>

#define BUFFER_SIZE 256
#define SERVER_SOCKET_PATH "tpf_unix_sock.server"
#define CLIENT_SOCKET_PATH "tpf_unix_sock.client"

int main()
{
	int client_sock = 0, len = 0;
	struct sockaddr_un server_sockaddr, client_sockaddr;
	char buff[BUFFER_SIZE];

	/******************/
	/* do server work */
	/******************/
	server_sockaddr.sun_family = AF_UNIX;
	strcpy(server_sockaddr.sun_path, SERVER_SOCKET_PATH);

	/*****************/
	/* do client work*/
	/*****************/
	if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		perror("Client error on socket() call");
		return 1;
	}

	client_sockaddr.sun_family = AF_UNIX;
	strcpy(client_sockaddr.sun_path, CLIENT_SOCKET_PATH);
	unlink(client_sockaddr.sun_path);
	len = sizeof(client_sockaddr);

	if (bind(client_sock, (struct sockaddr *)&client_sockaddr, len) == -1) 
	{
		perror("Client error on bind() call");
	}
	
	printf("Client trying to connect... \n");
	if (connect(client_sock, (struct sockaddr*)&server_sockaddr, len) == -1)
	{
		perror("Client error on connect() call");
		return 1;
	}
	printf("Client connected \n");

	/*********************************/
	/* sending and reciving messages */
	/*********************************/
	while (memset(buff, '\0', sizeof(buff)), printf(">"), fgets(buff, BUFFER_SIZE, stdin), !feof(stdin))
	{
		if (send(client_sock, buff, strlen(buff) * sizeof(char), 0) == -1)
		{
			perror("Client error on send() call");
		}

		if (strstr(buff, "quit") != NULL)
		{
			printf("client exit command received -> quitting \n");
			break;
		}

		memset(buff, 0, BUFFER_SIZE * sizeof(char));

		if ((len = recv(client_sock, buff, BUFFER_SIZE, 0)) > 0)
		{
			printf("Client data received: %s \n", buff);
		}
		else
		{
			if (len < 0)
			{
				perror("Client error on recv() call");
			}
			else
			{
				printf("Server socket closed \n");
				break;
			}

		}
	}
	close(client_sock);
	printf("bye! \n");

	return 0;
}
