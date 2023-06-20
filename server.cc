#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <cerrno>

#define BUFFER_SIZE 256
#define SERVER_SOCK_PATH "tpf_unix_sock.server"
static const unsigned int nIncomingConnections = 5;

int main()
{
	int server_sock = 0, client_sock = 0, len = 0;
	socklen_t sock_len = 0;
	struct sockaddr_un server_sockaddr, client_sockaddr;
	char recv_buff[BUFFER_SIZE], send_buff[BUFFER_SIZE];

	/******************/
	/* do server work */
	/******************/
	if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		perror("Server error on socket() call");
		return 1;
	}

	server_sockaddr.sun_family = AF_UNIX;
	strcpy(server_sockaddr.sun_path, SERVER_SOCK_PATH);
	unlink(server_sockaddr.sun_path);
	len = sizeof(server_sockaddr);

	if (bind(server_sock, (struct sockaddr*)&server_sockaddr, len) == -1)
	{
		perror("Server error on socket() call");
		return 1;
	}

	if (listen(server_sock, nIncomingConnections) == -1)
	{
		perror("Server error on listen() call");
	}

	
	/**************************************/
	/* reciving and sending back messages */
	/**************************************/
	bool bWaiting = true;
	while (bWaiting)
	{
		printf("Waiting for connection.... \n");
		if ((client_sock = accept(server_sock, (struct sockaddr*)&client_sockaddr, &sock_len)) == -1)
		{
			perror("Server error on accept() call");
			return 1;
		}

		printf("Server connected \n");
	
		do {
			memset(recv_buff, '\0', BUFFER_SIZE);
			len = recv(client_sock, recv_buff, BUFFER_SIZE, 0);
			if (len > 0)
			{
				printf("Data received: %d : %s \n", len, recv_buff);

				if (strstr(recv_buff, "quit_s") != NULL)
				{
					printf("Server exit command received -> quitting \n");
					bWaiting = false;
					break;
				}

				if (strstr(recv_buff, "quit") != NULL)
				{
					printf("Clinet is quitting \n");
					break;
				}

				memset(send_buff, '\0', BUFFER_SIZE);
				strcpy(send_buff, "Server got message: ");
				strcat(send_buff, recv_buff);

				if (send(client_sock, send_buff, strlen(send_buff) * sizeof(char), 0) == -1)
				{
					perror("Server error on send() call");
				}
			}
			else
			{
				perror("Server error on recv() call");
			}
		} while(len > 0);
	}
	close(client_sock);
	close(server_sock);
	return 0;
}


