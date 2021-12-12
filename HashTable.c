#include<math.h>
#include<stdio.h>
int WordNum=0;
int TotalWord=0;
//哈希表
struct LNode
{
	char* Code;
	char* Word;
	struct LNode* Next;
};
typedef struct LNode* LPtr;
struct FreNode
{
	char* Word;
	int total;
	struct FreNode* Next;
};
typedef struct FreNode* FNPtr;

int isPrime(int x)
{
	if(x<=1)return 0;
	else if(x==2)return 1;
	else
	{
		int i;
		for(i=2;i<=sqrt(x);i++)
		{
			if(x%i==0)return 0;
		}
		return 1;
	}
}
int NextPrime(int x)
{
	int Prime;
	int num=x;
	//(if x==1 return 2 但是不会被用到
	if(num%2==0)num++;
	while(1)
	{
		if(isPrime(num))return num;
		num+=2;
	}
}
int pow2(int n)
{
	return 1<<n;
}
int CodeToInt(char* Code)
{
	int res=0;
	int Weight=pow2(strlen(Code)-1);//Weight类型为int 此时最多能处理31位码字
	for(;(*Code)!='\0';Code++)
	{
		res+=Weight*((*Code)-'0');
		Weight/=2;
	}
	int x=strlen(Code);
	return res;
}
int CodeToInt8(char* Code)
{
	int res=0;
	int Weight=pow2(7);//Weight类型为int 此时最多能处理31位码字
	for(;(*Code)!='\0';Code++)
	{
		res+=Weight*((*Code)-'0');
		Weight/=2;
	}
	return res;
}
int GetLastx(unsigned char byte,int x)
{
	byte=byte<<(8-x);
	byte=byte>>(8-x);
	return (int)byte;
}
int Pow(int x,int y)
{
	int i;
	int res=1;
	for(i=1;i<=y;i++)
	{
		res*=x;
	}
	return res;
}
int Hash(char type,char* key,int thePrime)
{
	if(type=='c')//以Code为地址寻找
	{
  		int NumOfCode=CodeToInt(key);//这些都是int 最多处理31位码字
		int HashCode=NumOfCode%thePrime;
		return HashCode;
	}
	else if(type=='g')//以gbk编码的原文为地址找
	{
		int HashGBK;
		if(strlen(key)==2)
		{
			unsigned char c1Num=*key;
			unsigned char c2Num=*(key+1);
			int NumOfGBK=((c1Num-(1<<7))<<7)+(c2Num-(1<<7));
			HashGBK=NumOfGBK%thePrime;
		}
		else
		{
			unsigned char c1Num=*key;
			int NumOfGBK=c1Num*631;
			HashGBK=NumOfGBK%thePrime;
		}
		return HashGBK;
	}
	else if(type=='u')//utf-8的哈希
	{
		if(strlen(key)==1)
		{
			unsigned char c1Num=*key;
			int NumOfAChar=c1Num*631;
			return(NumOfAChar%thePrime);
			
		}
		else
		{
			int len=strlen(key);
			unsigned char* array=malloc(sizeof(unsigned char)*(len));
			int i;
			for(i=0;i<=len-1;i++)
			{
				array[i]=key[i];
			}
			int sum=0;
			int weight=Pow(1<<6,len-1);
			sum+=GetLastx(array[0],7-len)*weight;
			weight/=(1<<6);
			for(i=1;i<=len-1;i++)
			{
				sum+=GetLastx(array[i],6)*weight;
				weight/=(1<<6);
			}
			int res=sum%thePrime;
			return res;
		}
	}
}
struct hashTable
{
	LPtr* Heads;
	int MaxSize;
};
typedef struct hashTable* HashTable;
struct freHashTable
{
	FNPtr* Heads;
	int MaxSize;
};
typedef struct freHashTable* FHTable;
HashTable CreHashTable(int thePrime)
{
	HashTable HT=malloc(sizeof(struct hashTable));
	HT->MaxSize=thePrime;
	HT->Heads=malloc(sizeof(LPtr)*(HT->MaxSize));
	int i;
	for(i=0;i<=HT->MaxSize-1;i++) HT->Heads[i]=NULL;
	return HT;
}
FHTable CreFHTable(int thePrime)
{
	FHTable HT=malloc(sizeof(struct freHashTable));
	HT->MaxSize=thePrime;
	HT->Heads=malloc(sizeof(FNPtr)*(HT->MaxSize));
	int i;
	for(i=0;i<=HT->MaxSize-1;i++) HT->Heads[i]=NULL;
	return HT;
}
void HashInsert(HashTable HT,char* Code,char* Word,char type)
{
	LPtr LP=malloc(sizeof(struct LNode));
	LP->Code=Code;
	LP->Word=Word;
	LP->Next=NULL;
	int address;
	if(type=='g')address=Hash('g',Word,HT->MaxSize);
	else if(type=='u')address=Hash('u',Word,HT->MaxSize);
	else if(type=='c')address=Hash('c',Code,HT->MaxSize);
	if(HT->Heads[address])//冲突
	{
		LPtr P=HT->Heads[address];
		for(;(P->Next)!=NULL;P=P->Next);
		P->Next=LP;
	}
	else
	{
		HT->Heads[address]=LP;
	}
	
}
void FHInsert(FHTable FHT,char* Word)
{
	FNPtr FNP=malloc(sizeof(struct FreNode));
	FNP->Word=Word;
	FNP->Next=NULL;
	int address;
	if(UTF8)address=Hash('u',Word,FHT->MaxSize);
	else address=Hash('g',Word,FHT->MaxSize);
	if(FHT->Heads[address])//冲突或已有
	{
		FNPtr P=FHT->Heads[address];
		for(;P;P=P->Next)
		{
			if(strcmp(P->Word,FNP->Word)==0)
			{
				P->total++;
				free(FNP);
				return;
			}
		}
		P=FHT->Heads[address];
		for(;(P->Next)!=NULL;P=P->Next);
		FNP->total=1;
		WordNum++;
		P->Next=FNP;
		//return NULL;
	}
	else
	{
		FNP->total=1;
		WordNum++;
		FHT->Heads[address]=FNP;
	}
}
char* HashFind(HashTable HT,char* key,char type)
{
 	if(type=='g'||type=='u')//根据gbk(Word)找码(Code)
	{
  		int address=Hash(type,key,HT->MaxSize);
		if(HT->Heads[address])
		{
			LPtr P=HT->Heads[address];
			for(;P;P=P->Next)
			{
				if(strcmp(P->Word,key)==0)return P->Code;
			}
			return NULL;
		}
		else
		{
			return NULL;
		}
	}
	else if(type=='c')//code 解压时根据码(Code)找字符(Word)
	{
		int address=Hash('c',key,HT->MaxSize);
		if(HT->Heads[address])
		{
			LPtr P=HT->Heads[address];
			for(;P;P=P->Next)
			{
				if(strcmp(P->Code,key)==0)return P->Word;
			}
			return NULL;
		}
		else
		{
			return NULL;
		}
	}
}
void FreeHashTable(HashTable HT)
{
	int i;
	for(i=0;i<=HT->MaxSize-1;i++)
	{
		LPtr P=HT->Heads[i];
		LPtr Q;
		if(!P)continue;
		while(P->Next)
		{
			Q=P;
			P=P->Next;
			free(Q);
		}
		free(P);
	}
	free(HT->Heads);
	free(HT);
}
void FreeFHTable(FHTable FHT)
{
	int i;
	for(i=0;i<=FHT->MaxSize-1;i++)
	{
		FNPtr P=FHT->Heads[i];
		FNPtr Q;
		if(!P)continue;
		while(P->Next)
		{
			Q=P;
			P=P->Next;
			free(Q);
		}
		free(P);
	}
	free(FHT->Heads);
	free(FHT);
}
void HashMHInsert(MinHeap MH,FNPtr FNP)
{
	HuffmanTree HT=malloc(sizeof(struct HuffmanNode));
	HT->Word=FNP->Word;
	HT->total=FNP->total;
	HT->Left=HT->Right=NULL;
	//HT->level=1;
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
void FHTIntoHeap(FHTable FHT,MinHeap MH)
{
	int i;
	for(i=0;i<=FHT->MaxSize-1;i++)
	{
		FNPtr P=FHT->Heads[i];
		FNPtr Q;
		if(!P)continue;
		while(P->Next)
		{
			HashMHInsert(MH,P);
			P=P->Next;
		}
		HashMHInsert(MH,P);
	}

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

