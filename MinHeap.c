struct HuffmanNode
{
	char* Word;
	char* Code;
	int total;
	struct HuffmanNode* Left;
	struct HuffmanNode* Right;
};
typedef struct HuffmanNode* HuffmanTree;
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
//	int i;
//	for(i=0;i<=MaxSize;i++)
//	{
//		HP->Array[i]=malloc(sizeof(HuffmanTree));
//	}
	HP->Array[0]=malloc(sizeof(struct HuffmanNode));
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

void FreeMinHeap(MinHeap MH)
{
	free(MH->Array[0]);
	free(MH->Array);
	free(MH);
}
void FreeHuffmanTree(HuffmanTree HT)
{
	if(HT)
	{
		
		HuffmanTree Left=HT->Left;
		HuffmanTree Right=HT->Right;
		free(HT->Word);
		free(HT->Code);
		FreeHuffmanTree(Left);
		FreeHuffmanTree(Right);
	}
}
