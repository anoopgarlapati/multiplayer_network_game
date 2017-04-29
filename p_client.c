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


void printcard(card c)
{
	if(c.suite==1)
		printf("SPADES-");
	else if(c.suite==2)
		printf("CLUBS-");
	else if(c.suite==3)
		printf("HEARTS-");
	else
		printf("DIAMONDS-");
	if(c.rank==11)
		printf("J ");
	else if(c.rank==12)
		printf("Q ");
	else if(c.rank==13)
		printf("K ");
	else if(c.rank==14)
		printf("A ");
	else
		printf("%d ", c.rank);
}


int isPair(card * arr)
{
	int cntrank[15]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntrank[arr[i].rank]++;
	}
	int flag=0;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]==2)
			flag=1;
	}
	return flag;
}

int isTwoPair(card * arr)
{
	int cntrank[15]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntrank[arr[i].rank]++;
	}
	int flag=0;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]==2)
			flag+=1;
	}
	if(flag>=2)
		return 1;
	else
		return 0;
}

int isThreeofakind(card * arr)
{
	int cntrank[15]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntrank[arr[i].rank]++;
	}
	int flag=0;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]==3)
			flag=1;
	}
	return flag;
}

int isStraight(card * arr)
{
	int cntrank[15]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntrank[arr[i].rank]++;
		if(arr[i].rank==14)
			cntrank[1]++;
	}
	int currlen=0, maxlen=1;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]>=1)
		{
			currlen++;
			if(currlen>maxlen)
				maxlen = currlen;
		}
		else
			currlen=0;
	}

	if(maxlen>=5)
		return 1;
	else
		return 0;
}

int isFlush(card * arr)
{
	int cntsuite[5]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntsuite[arr[i].suite]++;
	}
	int flag=0;
	for(i=0; i<5; i++)
	{
		if(cntsuite[i]>=5)
			flag=1;
	}
	return flag;
}

int isFullHouse(card * arr)
{
	int cntrank[15]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntrank[arr[i].rank]++;
	}
	int flag3=0,flag2=0;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]==3 && flag3==0)
			flag3=1;
		else if(cntrank[i]>=2)
			flag2=1;
	}
	if(flag3==1 && flag2==1)
		return 1;
	else
		return 0;
}

int isFourofakind(card * arr)
{
	int cntrank[15]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntrank[arr[i].rank]++;
	}
	int flag=0;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]==4)
			flag=1;
	}
	return flag;
}

int isStraightFlush(card * arr)
{
	if(!isStraight(arr) || !isFlush(arr))
		return 0;

	int cntsuite[5]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntsuite[arr[i].suite]++;
	}
	int flushsuite=0;
	for(i=0; i<5; i++)
	{
		if(cntsuite[i]>=5)
			flushsuite=i;
	}
	card newhand[7];
	int j=0;
	for(i=0; i<7; i++)
	{
		if(arr[i].suite==flushsuite)
		{
			newhand[j++]=arr[i];
		}
	}

	int cntrank[15]={0};
	for(i=0; i<j; i++)
	{
		cntrank[newhand[i].rank]++;
		if(newhand[i].rank==14)
			cntrank[1]++;
	}
	int currlen=0, maxlen=1;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]>=1)
		{
			currlen++;
			if(currlen>maxlen)
				maxlen = currlen;
		}
		else
			currlen=0;
	}

	if(maxlen>=5)
		return 1;
	else
		return 0;
}

int isRoyalFlush(card * arr)
{
	if(!isStraight(arr) || !isFlush(arr))
		return 0;

	int cntsuite[5]={0};
	int i;
	for(i=0; i<7; i++)
	{
		cntsuite[arr[i].suite]++;
	}
	int flushsuite=0;
	for(i=0; i<5; i++)
	{
		if(cntsuite[i]>=5)
			flushsuite=i;
	}
	card newhand[7];
	int j=0;
	for(i=0; i<7; i++)
	{
		if(arr[i].suite==flushsuite)
		{
			newhand[j++]=arr[i];
		}
	}

	int cntrank[15]={0};
	for(i=0; i<j; i++)
	{
		cntrank[newhand[i].rank]++;
		if(newhand[i].rank==14)
			cntrank[1]++;
	}
	int currlen=0, maxlen=1;
	for(i=0; i<15; i++)
	{
		if(cntrank[i]>=1)
		{
			currlen++;
			if(currlen>maxlen)
				maxlen = currlen;
		}
		else
			currlen=0;
	}

	if(maxlen>=5 && cntrank[14]==1 && cntrank[13]==1 && cntrank[12]==1 && cntrank[11]==1 && cntrank[10]==1)
		return 1;
	else
		return 0;
}

int highCard(card * arr)
{
	int i,hc=2;
	for(i=0; i<7; i++)
	{
		hc = max(hc,arr[i].rank);
	}
	return hc;
}

int handRank(card * arr)
{
	if(isRoyalFlush(arr))
		return 37;
	if(isStraightFlush(arr))
		return 36;
	if(isFourofakind(arr))
		return 35;
	if(isFullHouse(arr))
		return 34;
	if(isFlush(arr))
		return 33;
	if(isStraight(arr))
		return 32;
	if(isThreeofakind(arr))
		return 31;
	if(isTwoPair(arr))
		return 30;
	if(isPair(arr))
	{
		return 13+highCard(arr);
	}
	return highCard(arr);
}

char winhand[20];
void getwinhand(int hr)
{
	if(hr==37)
		strcpy(winhand,"Royal Flush");
	else if(hr==36)
		strcpy(winhand,"Straight Flush");
	else if(hr==35)
		strcpy(winhand,"Four of a Kind");
	else if(hr==34)
		strcpy(winhand,"Full House");
	else if(hr==33)
		strcpy(winhand,"Flush");
	else if(hr==32)
		strcpy(winhand,"Straight");
	else if(hr==31)
		strcpy(winhand,"Three of a Kind");
	else if(hr==30)
		strcpy(winhand,"Two Pair");
	else if(hr>=15 && hr<=29)
		strcpy(winhand,"Pair");
	else
		strcpy(winhand,"High Card");

}

int main(int * argc, char * argv[])
{
	char IP[20];
	int PORT;
	printf("Enter IP address of the game server: ");
	scanf("%s",IP);
	printf("Enter PORT number of the game server: ");
	scanf("%d",&PORT);


	struct sockaddr_in sock_var;
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	char str_client[20], str_server[20], servermsg[20];

	sock_var.sin_addr.s_addr = inet_addr(IP);
	sock_var.sin_port = htons(PORT);
	sock_var.sin_family = AF_INET;

	if(connect(serverfd, (struct sockaddr *)&sock_var, sizeof(sock_var))>=0)
	{
		//Supervisor
		card mycard1,mycard2;
		int amiactive=1;
		player myself;

		printf("Connected to server.\n");
		printf("Enter your name (max 20 characters): ");
		scanf("%s", str_client);
		int sen = send(serverfd, str_client, strlen(str_client),0);
		if(sen<1)
		{
			amiactive=0;
			printf("Disconnected from the server.\n");
			
		}
		int rec = recv(serverfd, &myself, sizeof(myself), MSG_WAITALL);
		if(rec<1)
		{
			printf("Disconnected from the server.\n");
			amiactive=0;
		}
		else
		{
			printf("Communication successful.\n");
		}
		
		printf("Round 1 starts in 30 seconds.\n");
		//Rounds - Game starts
		int roundnum=1, thisround=1;
		while(amiactive==1)
		{
			//Round


			thisround=1;
			msg1 newmsg1;
			rec = recv(serverfd,&newmsg1,sizeof(newmsg1),MSG_WAITALL);
			rec = recv(serverfd,&myself,sizeof(myself),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}
			if(newmsg1.c=='a')
				amiactive=1;
			else
			{
				amiactive=0;
				break;
			}

			msg1 msg1_2;
			rec = recv(serverfd,&msg1_2,sizeof(msg1_2),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}
			if(msg1_2.c=='@')
				break;

			rec = recv(serverfd,&mycard1,sizeof(mycard1),MSG_WAITALL);
			rec = recv(serverfd,&mycard2,sizeof(mycard2),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}

			printf("Your cards are: ");
			printcard(mycard1);
			printf(" and ");
			printcard(mycard2);
			printf("\n");
			// printf("fdsfdf");
			printf("Call 20,000 or Fold. Enter 'c' for call and 'f' for fold: ");
			
			msg1 pretempmsg;
			rec = recv(serverfd,&pretempmsg,sizeof(pretempmsg),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}

			if(pretempmsg.c=='y')
			{
				char temp;
				scanf(" %c",&temp);
				// printf("%c hgjghj \n", temp);
				msg1 tempmsg;
				tempmsg.c = temp;
				sen=send(serverfd,&tempmsg,sizeof(tempmsg),0);
				if(sen<1)
				{
					amiactive=0;
					printf("Disconnected from the server.\n");
					break;
				}
				if(temp=='f')
					thisround=0;
			}

			//Turn 2: Flop.
			//Receive three cards from the server.
			card x,y,z;
			rec = recv(serverfd,&x,sizeof(x),MSG_WAITALL);
			rec = recv(serverfd,&y,sizeof(y),MSG_WAITALL);
			rec = recv(serverfd,&z,sizeof(z),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}

			printf("The cards on the table are: ");
			printcard(x);
			printf(", ");
			printcard(y);
			printf(" and ");
			printcard(z);
			printf("\n");

			if(thisround)
			{
				msg1 pretempmsg2;
				rec=recv(serverfd,&pretempmsg2,sizeof(pretempmsg2),MSG_WAITALL);
				if(rec<1)
				{
					amiactive=0;
					printf("Disconnected from the server.\n");
					break;
				}
				if(pretempmsg2.c=='y')
				{
					char temp;
					printf("Call All-In or Fold. Enter 'c' for Call and 'f' for Fold: ");
					scanf(" %c",&temp);
					msg1 tempmsg2;
					tempmsg2.c = temp;
					if(temp=='f')
						thisround=0;
					sen = send(serverfd,&tempmsg2,sizeof(tempmsg2),0);
					if(sen<1)
					{
						amiactive=0;
						printf("Disconnected from the server.\n");
						break;
					}
				}
			}

			//Receive last two cards on the table.
			card a,b;
			rec = recv(serverfd,&a,sizeof(a),MSG_WAITALL);
			rec = recv(serverfd,&b,sizeof(b),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}
			printf("The cards on the table are: ");
			printcard(x);
			printf(", ");
			printcard(y);
			printf(", ");
			printcard(z);
			printf(", ");
			printcard(a);
			printf(" and ");
			printcard(b);
			printf("\n");

			if(thisround)
			{
				card myhand[7];
				myhand[0]=mycard1;
				myhand[1]=mycard2;
				myhand[2]=x;
				myhand[3]=y;
				myhand[4]=z;
				myhand[5]=a;
				myhand[6]=b;
				int hr = handRank(myhand);
				msg3 hrmsg;
				hrmsg.val = hr;
				sen = send(serverfd,&hrmsg,sizeof(hrmsg),0);
				if(sen<1)
				{
					amiactive=0;
					printf("Disconnected from the server.\n");
					break;
				}
			}

			msg2 msg2_;
			rec = recv(serverfd,&msg2_,sizeof(msg2_),MSG_WAITALL);
			if(rec<1)
			{
				amiactive=0;
				printf("Disconnected from the server.\n");
				break;
			}
			getwinhand(msg2_.hr);
			for(int i=0; i<msg2_.size; i++)
			{
				printf("%s, ",msg2_.arr[i].name);
			}
			printf("win(s) the round with %s\n", winhand);

			printf("Round %d has ended.\n", roundnum);

			roundnum++;
		}

		if(amiactive && myself.chips>0)
		{
			printf("You won the game. CONGRATULATIONS!!!\n");
		}
		else
		{
			printf("You are out of chips. You lost. Better luck next time.!!!\n");
		}



		close(serverfd);
	}
	else
	{
		printf("socket creation failed\n");
	}

	return 0;
}
