#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define IP 		"127.0.0.1"
#define PORT 	8081

typedef struct g_player {
	int chips;
	int cfd;
	char name[20];
}player;

int cfd;

int main()
{
	struct sockaddr_in sock_var;
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	char str_client[20], str_server[20];

	sock_var.sin_addr.s_addr = inet_addr(IP);
	sock_var.sin_port = PORT;
	sock_var.sin_family = AF_INET;

	if(connect(serverfd, (struct sockaddr *)&sock_var, sizeof(sock_var))>=0)
	{
		printf("connected to server %d\n", serverfd);
		printf("Enter your name: \n");
		scanf("%s", str_client);
		int sen = send(serverfd, str_client, strlen(str_client),0);
		int rec = recv(serverfd, str_server, strlen(str_server),0);
		if(rec==0)
		{
			printf("Disconnected from the server.\n");
		}
		else if(rec>0)
		{
			if(str_server[0]=='E')
				printf("Communication successful.\n");
		}

		close(serverfd);
	}
	else
	{
		printf("socket creation failed\n");
	}

	return 0;
}