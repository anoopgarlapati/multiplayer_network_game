#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//suites 1-SPADES, 2-CLUBS, 3-HEARTS, 4-DIAMONDS, rank 2345678910JQKA A-14
struct card_ {
	int suite,rank;
};
typedef struct card_ card;

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

int main()
{
	/*
	card a,b,c,d,e,f,g;
	int i;	
	a.suite=1,a.rank=6;
	b.suite=2,b.rank=14;
	c.suite=3,c.rank=12;
	d.suite=4,d.rank=11;
	e.suite=4,e.rank=11;
	e.suite=4,e.rank=11;
	// printcard(a);
	// printcard(b);
	// printcard(c);
	// printcard(d);
	card arr[7];
	arr[0].suite=4,arr[0].rank=13;
	arr[1].suite=4,arr[1].rank=2;
	arr[2].suite=4,arr[2].rank=3;
	arr[3].suite=4,arr[3].rank=12;
	arr[4].suite=4,arr[4].rank=11;
	arr[5].suite=4,arr[5].rank=10;
	arr[6].suite=4,arr[6].rank=14;

	printf("%d", isRoyalFlush(arr));
	*/

	//create newDeck
	srand(time(NULL));
	card deck[52];
	int i;
	for(i=0; i<52; i++)
	{
		deck[i].suite = i/13+1;
		deck[i].rank = i%13+2;
	}

	//printDeck
	for(i=0; i<52; i++)
	{
		printcard(deck[i]);
		printf("\n");
	}

	//shuffleDeck
	shuffleDeck(deck);

	//printDeck
	for(i=0; i<52; i++)
	{
		printcard(deck[i]);
		printf("\n");
	}


	return 0;
}