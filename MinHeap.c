//只取Word和total就可以

struct HuffmanNode
{
	char* Word;
	char* Code;
	int total;
	struct HuffmanNode* Left;
	struct HuffmanNode* Right;
	int level;
};
typedef struct HuffmanNode* HuffmanTree;
//——————————————Test——————————————
struct Q2Node
{
	HuffmanTree anPtr;
	struct Q2Node* Next;
};
typedef struct Q2Node* q2nPtr;
//typedef qnPtr Queue;
struct q2ueue
{
	q2nPtr head;
	q2nPtr tail;
};
typedef struct q2ueue* Q2ueue;

Q2ueue Q2cre()
{
	Q2ueue q=malloc(sizeof(struct q2ueue));
	q->head=q->tail=NULL;
	return q;
}

Q2ueue Q2add(Q2ueue theQ,HuffmanTree anPtr)//入队
{
	q2nPtr qn=malloc(sizeof(struct Q2Node));
	qn->anPtr=anPtr;
	qn->Next=NULL;
	if(theQ->head==NULL)
	{
		theQ->head=theQ->tail=qn;
	}
	else
	{
		theQ->tail->Next=qn;
		theQ->tail=theQ->tail->Next;
	}
	return theQ;
}

HuffmanTree Q2del(Q2ueue theQ)//出队
{
	q2nPtr head=theQ->head;
	theQ->head=head->Next;
	HuffmanTree a=head->anPtr;
	free(head);
	return a;
}
void printHuffmanBetter(HuffmanTree avlt)//
{
	if(avlt)
	{
		HuffmanTree ap;
		Q2ueue Q2=Q2cre();
		Q2add(Q2,avlt);
		int nowlevel=1;
		while(Q2->head!=NULL)
		{
			ap=Q2del(Q2);
			if(ap->level>nowlevel)
			{
				nowlevel++;
				printf("\n");
			}

			if(!(ap->Word))printf("NULL");
			else printf("%s",ap->Word);
			printf("(%d)",ap->total);
			printf("(L:%d) ",ap->level);
			printf("Code:%s\n",ap->Code);
			
			if(ap->Left)
			{
				Q2add(Q2,ap->Left);
			}
			if(ap->Right)
			{
				Q2add(Q2,ap->Right);
			}

		}
		printf("\n");
	}
}
//——————————————Test——————————————
void ChangeAllHuffmanLevel(HuffmanTree HT,int x)
{
	if(HT)
	{
		HT->level+=x;
		ChangeAllHuffmanLevel(HT->Left,x);
		ChangeAllHuffmanLevel(HT->Right,x);
	}
}
void PrintHuffman(HuffmanTree HT)
{
	if(HT)
	{
		if(!(HT->Word))printf("NULL");
		else printf("%s",HT->Word);
		printf("(%d)",HT->total);
		printf("(L:%d) ",HT->level);
		printf("Code:%s\n",HT->Code);
		PrintHuffman(HT->Left);
		PrintHuffman(HT->Right);
	}
}
struct minHeap
{
	HuffmanTree* Array;
	int Total;
	int MaxSize;
};
typedef struct minHeap* MinHeap;
int isMHEmpty(MinHeap MH)
{
	if(MH->Total==0)return 1;
	else return 0;
}
MinHeap CreateMinHeap(int MaxSize)//0号是哨兵 不用
{
	MinHeap HP=malloc(sizeof(struct minHeap));
	HP->Total=0;
	HP->MaxSize=MaxSize;
	//HP->Array=malloc(sizeof(struct HuffmanNode)*(MaxSize+1));
	//数组里全是结构体应该...?
	HP->Array=malloc(sizeof(HuffmanTree)*(MaxSize+1));
	int i;
	for(i=0;i<=MaxSize;i++)
	{
		HP->Array[i]=malloc(sizeof(struct HuffmanNode));
	}
	HP->Array[0]->Word=malloc(sizeof(char)*1);
	HP->Array[0]->Word[0]='\0';
	HP->Array[0]->total=-1;
	return HP;//又尼玛不返回
}
int HTcmp(HuffmanTree T1,HuffmanTree T2)
{
	if(T1->total<T2->total)return -1;
	else if(T1->total>T2->total)return 1;
	else
	{
		if(!(T1->Word))return -1;
		else if(!(T2->Word))return 1;
		else
		{
			if(strcmp(T1->Word,T2->Word)<0)return -1;
			else if(strcmp(T1->Word,T2->Word)>0)return 1;
		}
	}
	return 0;
}
void MHInsert(MinHeap MH,AVLTree AVLT)
{
	HuffmanTree HT=malloc(sizeof(struct HuffmanNode));
	//HT->Word=AVLT->Word;
	HT->Word=malloc(sizeof(char)*(1+strlen(AVLT->Word)));
	strcpy(HT->Word,AVLT->Word);//不知\0的问题
	HT->total=AVLT->total;
	HT->Left=HT->Right=NULL;
	HT->level=1;
	HT->Code=NULL;

	MH->Total++;
	//MH->Array[MH->Total]=HT;
	int current=MH->Total;
	while(HTcmp(HT,MH->Array[current/2])<0)
	{
		MH->Array[current]=MH->Array[current/2];
		current=current/2;
	}
	MH->Array[current]=HT;
}
void InsertIntoHeap(AVLTree Frequency,MinHeap MH)
{
	if(Frequency)
	{
		MHInsert(MH,Frequency);
		InsertIntoHeap(Frequency->Left,MH);
		InsertIntoHeap(Frequency->Right,MH);
	}
}
void MHHuffmanInsert(MinHeap MH,HuffmanTree HT)
{
	MH->Total++;
	//MH->Array[MH->Total]=HT;
	int current=MH->Total;
	while(HTcmp(HT,MH->Array[current/2])<0)
	{
		MH->Array[current]=MH->Array[current/2];
		current=current/2;
	}
	MH->Array[current]=HT;
}
HuffmanTree DeleteMin(MinHeap MH)
{
	HuffmanTree res=MH->Array[1];
	HuffmanTree Tree=MH->Array[MH->Total];
	MH->Total--;
	
	int current=1;
	int minChild=2*current;
	if((current*2+1<=MH->MaxSize)&&HTcmp(MH->Array[2*current],MH->Array[2*current+1])>0)minChild++;

//	while(current<=MH->Total/2&&(HTcmp(MH->Array[current],MH->Array[minChild])>0))
	while(current<=MH->Total/2&&(HTcmp(Tree,MH->Array[minChild])>0))
	{
		MH->Array[current]=MH->Array[minChild];
		current=minChild;
		minChild=2*current;
		if((current*2+1<=MH->MaxSize)&&HTcmp(MH->Array[2*current],MH->Array[2*current+1])>0)minChild++;
	}
	MH->Array[current]=Tree;
	return res;
}

HuffmanTree HuffmanCre(MinHeap MH)
{
	while(MH->Total>=2)
	{
		HuffmanTree HT1=DeleteMin(MH);
		HuffmanTree HT2=DeleteMin(MH);
		HuffmanTree Generate=malloc(sizeof(struct HuffmanNode));
		ChangeAllHuffmanLevel(HT1,1);
		ChangeAllHuffmanLevel(HT2,1);
		Generate->level=HT1->level-1;
		Generate->Left=HT1;
		Generate->Right=HT2;
		Generate->total=HT1->total+HT2->total;
		Generate->Word=NULL;
		Generate->Code=NULL;
		MHHuffmanInsert(MH,Generate);
	}
	return DeleteMin(MH);
}
void SetCode(HuffmanTree HT,char position,int len,char* ParentCode)
{
	if(HT)
	{
		if(position=='f')
		{
			HT->Code=malloc(sizeof(char)*1);
			HT->Code[0]='\0';
		}
		else if(position=='l')
		{
			HT->Code=malloc(sizeof(char)*(len+1+1));
			if(ParentCode[0]!='\0')strcpy(HT->Code,ParentCode);
			strcpy(len+HT->Code,"1");
			HT->Code[len+1]='\0';
		}
		else if(position=='r')
		{
			HT->Code=malloc(sizeof(char)*(len+1+1));
			if(ParentCode[0]!='\0')strcpy(HT->Code,ParentCode);
			strcpy(len+HT->Code,"0");
			HT->Code[len+1]='\0';
		}
		//printf("%s\n",HT->Word);
		SetCode(HT->Left,'l',strlen(HT->Code),HT->Code);
		SetCode(HT->Right,'r',strlen(HT->Code),HT->Code);
	}
}
int GetHuffmanHeight(HuffmanTree HT)
{
	if(HT)return 1+max(GetHuffmanHeight(HT->Left),GetHuffmanHeight(HT->Right));
	else return 0;
}
void InsertToHash(HuffmanTree HT,HashTable HashT,char type)
{
	if(HT)
	{
		if(HT->Word)
		{
			HashInsert(HashT,HT->Code,HT->Word,type);
		}
		InsertToHash(HT->Left,HashT,type);
		InsertToHash(HT->Right,HashT,type);
	}
}
