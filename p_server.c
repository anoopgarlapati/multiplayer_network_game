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

#define max(a,b) (a>b)?a:b

typedef struct g_player {
	int chips;
	int chips_curr;
	int cfd;
	int is_active;
	int thisround;
	int handrank;
	char name[20];
}player;

//suites 1-SPADES, 2-CLUBS, 3-HEARTS, 4-DIAMONDS, rank 2345678910JQKA A-14
struct g_card {
	int suite,rank;
};
typedef struct g_card card;

typedef struct g_msg1 {
	char c;
}msg1;

typedef struct g_msg2 {
	int size;
	int hr;
	player arr[17];
}msg2;

msg2 newmsg2(int x, int hr, player * arr)
{
	msg2 newmsg2;
	newmsg2.size = x;
	newmsg2.hr = hr;
	for(int i=0; i<x; i++)
	{
		newmsg2.arr[i] = arr[i];
	}
	return newmsg2;
}


typedef struct g_msg3 {
	int val;
}msg3;

player players[17];
int noofplayers=0;
int roundnum=0;
int noofactiveplayers=0;

player new_player(int cfd, char * name)
{
	player newg;
	newg.chips = 200000;
	newg.chips_curr = 0;
	newg.cfd = cfd;
	newg.is_active = 1;
	newg.thisround=1;
	newg.handrank = 1;
	strcpy(newg.name, name);
	return newg;
}

//Shuffle Deck - Fisher-Yates Algorithm
void shuffleDeck(card * deck)
{
	int i;
	for(i=0; i<52; i++)
	{
		card temp = deck[i];
		int j = rand()%52;
		deck[i] = deck[j];
		deck[j] = temp;
	}
}

void broadcast(const void * buf, size_t len)
{
	int i;
	for(i=0; i<noofplayers; i++)
	{
		if(players[i].is_active==1)
		{
			int sen = send(players[i].cfd,buf,len,0);
			if(sen<1)
				players[i].is_active=0;
		}
	}
}

int rec=0,sen=0;

int main()
{
	char IP[20];
	int PORT;
	printf("Enter IP address: ");
	scanf("%s", IP);
	printf("Enter PORT number: ");
	scanf("%d",&PORT);


	//Socket connections
	struct sockaddr_in sock_var;
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	int clientfd, cfd;

	sock_var.sin_addr.s_addr = inet_addr(IP);
	sock_var.sin_port = htons(PORT);
	sock_var.sin_family = AF_INET;

	struct sockaddr_in socket2;
	int serverSocket2 = socket(AF_INET, SOCK_STREAM, 0);
	int cfd2;

	if(bind(serverfd,(struct sockaddr *)&sock_var, sizeof(sock_var))>=0)
	{
		printf("Socket created successfully\n");
		listen(serverfd,17);

		//Supervisor
		
		//Get players' information
		for(int i=0; i<17; i++)
		{
			int res;
			struct timeval tv;
			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(serverfd, &readfds);

			tv.tv_sec = (long)20;
			tv.tv_usec = (long)0;

			char str[20];
			for(int j=0; j<20; j++)
				str[j] = '\0';

			res = select(serverfd+1, &readfds, (fd_set *)0, (fd_set *)0, &tv);
			if(res==-1)
			{
				perror("select");
				return 1;
			}
			else if(res==0)
			{
				//timeout
				break;
			}
			else
			{
				clientfd = accept(serverfd,NULL,NULL);
				printf("Connected to client #%d\n", clientfd);
				if(recv(clientfd,str,20,0)>0)
				{
					printf("Client #%d's name is %s\n", clientfd, str);
					player newg = new_player(clientfd,str);
					players[noofplayers++] = newg;
					sen = send(clientfd,&newg,sizeof(newg),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",clientfd);
						newg.is_active=0;
					}
				}
				else
				{
					printf("Client #%d has disconnected.\n", clientfd);
				}
			}
		}
		
		//Create new deck - cards are in order, they are shuffled in each round later.
		srand(time(NULL));
		card deck[52];
		int iter;
		for(iter=0; iter<52; iter++)
		{
			deck[iter].suite = iter/13+1;
			deck[iter].rank = iter%13+2;
		}

		//Rounds - Game starts
		noofactiveplayers=noofplayers;
		int pot = 0;
		while(1)
		{
			pot=0;
			//Shuffle the deck at the start of each round.
			shuffleDeck(deck);

			//Set chips_curr to 0, thisround to 1 for each active player.
			for(int i=0; i<noofplayers; i++)
			{
				if(players[i].is_active)
				{
					players[i].chips_curr=0;
					players[i].thisround=1;
					char mssg='a';
					if(players[i].chips==0)
					{
						mssg='i';
						players[i].is_active=0;
					}
					msg1 newmsg1;
					newmsg1.c = mssg;
					sen = send(players[i].cfd,&newmsg1,sizeof(newmsg1),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",players[i].cfd);
						players[i].is_active=0;

					}
					printf("%d %c\n", players[i].chips, mssg);
					sen = send(players[i].cfd,&(players[i]),sizeof((players[i])),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",players[i].cfd);
						players[i].is_active=0;
					}
				}
			}

			int noofactive=0;
			for(int i=0; i<noofplayers; i++)
			{
				if(players[i].is_active)
					noofactive++;
			}
			char ch;
			if(noofactive<=1)
				ch='@';
			else
				ch='#';
			//Determine if game is over or not. @=>over, #=>not over.
			msg1 msg1_2;
			msg1_2.c=ch;
			broadcast(&msg1_2,sizeof(msg1_2));
			if(ch=='@')
				break;


			//Turn 1: Pre-Flop. Ante-20000 chips
			//Distribute two cards to each player
			int cardpos=0;
			for(int i=0;i<noofplayers; i++)
			{
				if(players[i].is_active)
				{
					card x = deck[cardpos];
					cardpos++;
					card y = deck[cardpos];
					cardpos++;
					sen = send(players[i].cfd,&x,sizeof(x),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",players[i].cfd);
						players[i].is_active=0;
						continue;
					}
					sen = send(players[i].cfd,&y,sizeof(y),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",players[i].cfd);
						players[i].is_active=0;
						continue;
					}
				}
			}
			//Players make their bets - Either Call 20,000 or Fold.
			for(int i=0; i<noofplayers; i++)
			{
				if(players[i].is_active)
				{
					msg1 pretempmsg;
					pretempmsg.c = 'y';
					int sen;
					sen = send(players[i].cfd,&pretempmsg,sizeof(pretempmsg),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",players[i].cfd);
						players[i].is_active=0;
						continue;
					}
					msg1 tempmsg;
					
					rec=recv(players[i].cfd,&tempmsg,sizeof(tempmsg),MSG_WAITALL);
					if(rec>0)
					{
						// printf("%d %d\n",rec,sizeof(tempmsg));
						// printf("%c\n",tempmsg.c);
						if(tempmsg.c=='c')
						{
							players[i].chips_curr+=20000;
							players[i].chips-=20000;
							if(players[i].chips<0)
								players[i].chips=0;
							pot+=20000;
						}
						else
						{
							players[i].thisround=0;
						}
						
					}
					else
					{
						printf("Player #%d has been removed from the game due to poor connectivity.\n", players[i].cfd);
						players[i].is_active=0;
					}
				}
			}

			//Turn 2: Flop.
			//Show the three flop cards to all the players.
			card x = deck[cardpos];
			cardpos++;
			card y = deck[cardpos];
			cardpos++;
			card z = deck[cardpos];
			cardpos++;
			broadcast(&x,sizeof(x));
			broadcast(&y,sizeof(y));
			broadcast(&z,sizeof(z));

			//Players make their bets - Either All-In or Fold.
			for(int i=0; i<noofplayers; i++)
			{
				if(players[i].is_active && players[i].thisround)
				{
					msg1 pretempmsg2;
					pretempmsg2.c = 'y';
					sen = send(players[i].cfd,&pretempmsg2,sizeof(pretempmsg2),0);
					if(sen<1)
					{
						printf("Client #%d has been removed from the game due to poor connectivity.\n",players[i].cfd);
						players[i].is_active=0;
						continue;
					}
					msg1 tempmsg2;
					rec = recv(players[i].cfd,&tempmsg2,sizeof(tempmsg2),MSG_WAITALL);
					if(rec>0)
					{

						if(tempmsg2.c=='c')
						{
							players[i].chips_curr+=players[i].chips;
							pot+=players[i].chips;
							players[i].chips=0;
						}
						else
						{
							players[i].thisround=0;
						}
					}
					else
					{
						printf("Player #%d has been removed from the game due to poor connectivity.\n", players[i].cfd);
						players[i].is_active=0;
					}
				}
			}

			//Reveal last two cards.
			card a = deck[cardpos];
			cardpos++;
			card b = deck[cardpos];
			cardpos++;
			broadcast(&a,sizeof(a));
			broadcast(&b,sizeof(b));



			//All turns over. Determine Winner of the round.
			int maxhandrank=0;
			for(int i=0; i<noofplayers; i++)
			{
				if(players[i].is_active && players[i].thisround)
				{
					msg3 hrmsg;
					rec=recv(players[i].cfd, &hrmsg, sizeof(hrmsg),MSG_WAITALL);
					if(rec>0)
					{
						players[i].handrank=hrmsg.val;
						maxhandrank = max(maxhandrank,hrmsg.val);
					}
					else
					{
						printf("Player #%d has been removed from the game due to poor connectivity.\n", players[i].cfd);
						players[i].is_active=0;
					}
				}
			}
			int noofwinners=0;
			for(int i=0;i<noofplayers;i++)
			{
				if(players[i].is_active && players[i].thisround)
				{
					if(players[i].handrank==maxhandrank)
						noofwinners++;
				}
			}
			player round_winners[noofwinners];
			int ix=0;
			for(int i=0; i<noofplayers; i++)
			{
				if(players[i].is_active && players[i].thisround)
				{
					if(players[i].handrank==maxhandrank)
					{
						round_winners[ix] = players[i];
						players[i].chips+=pot/noofwinners;
						ix++;
					}
				}
			}
			msg2 msg2_= newmsg2(noofwinners,maxhandrank,round_winners);
			broadcast(&msg2_,sizeof(msg2_));

			roundnum++;
		}

		player winner;
		for(int x=0; x<noofplayers; x++)
		{
			if(players[x].is_active==1)
			{
				winner = players[x];
				break;
			}
		}

		char gameover = '!';
		msg1 gomsg1;
		gomsg1.c = gameover;
		broadcast(&gomsg1,sizeof(gomsg1));
		broadcast(&winner,sizeof(winner));
		printf("GAME OVER\n");

		close(serverfd);
	}
	else
	{
		printf("Socket creation failed\n");
	}



	return 0;
}