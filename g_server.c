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

int main()
{
	struct sockaddr_in sock_var;
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	int clientfd, cfd;

	sock_var.sin_addr.s_addr = inet_addr(IP);
	sock_var.sin_port = PORT;
	sock_var.sin_family = AF_INET;

	struct sockaddr_in socket2;
	int serverSocket2 = socket(AF_INET, SOCK_STREAM, 0);
	int cfd2;

	if(bind(serverfd,(struct sockaddr *)&sock_var, sizeof(sock_var))>=0)
	{
		printf("socket created successfully\n");
		listen(serverfd,7);

		for(int i=0; i<100; i++)
		{
			char str[20];
			clientfd = accept(serverfd, NULL, NULL);
			printf("connected to client %d\n", clientfd);
			if(recv(clientfd,str,20,0)>0)
			{
				printf("Client #%d 's name is: %s.\n", clientfd, str);
				send(clientfd,"Exit",4,0);
			}
			else if(recv(clientfd,str,20,0)==0)
			{
				printf("Client %d has disconnected.\n", clientfd);
			}
		}
		close(serverfd);
	}
	else
	{
		printf("socket creation failed\n");
	}



	return 0;
}