#include<math.h>
#include<stdio.h>
//哈希表
struct LNode
{
	char* Code;
	char* Word;
	struct LNode* Next;
};
typedef struct LNode* LPtr;
//struct list
//{
//	LPtr []
//};
//毕竟不是List 应该不用Head和Tail;
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
}int CodeToInt8(char* Code)
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
}
struct hashTable
{
	LPtr* Heads;
	int MaxSize;
};
typedef struct hashTable* HashTable;
HashTable CreHashTable(int thePrime)
{
	HashTable HT=malloc(sizeof(struct hashTable));
	HT->MaxSize=thePrime;
	HT->Heads=malloc(sizeof(LPtr)*(HT->MaxSize));
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
char* Find(HashTable HT,char* key,char type)
{
 	if(type=='g')//根据gbk(Word)找码(Code)
	{
  		int address=Hash('g',key,HT->MaxSize);
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
//int main()
//{
//	printf("%d",CodeToInt("0010011110"));
//}
//int main()
//{
////	char* x=malloc(sizeof(char)*30);
////    gets(x);
////	printf("%d",Hash('g',x,233));
//	//printf("%d",NextPrime(626));
//	HashTable HT=CreHashTable(5);
//	char** array=malloc(sizeof(char*)*10);
//	char** codes=malloc(sizeof(char*)*10);
//	int i;
//	for(i=0;i<=2;i++)
//	{
//		array[i]=malloc(sizeof(char)*30);
//		codes[i]=malloc(sizeof(char)*30);
//		scanf("%s",array[i]);
//		scanf("%s",codes[i]);
//	}
//	
//	for(i=0;i<=2;i++)
//	{
//		HashInsert(HT,codes[i],array[i],'c');
//	}
//	printf("Find:\n");
//	for(i=1;i<=3;i++)
//	{
//		char* x=malloc(sizeof(char)*3);
//		scanf("%s",x);
//		char* res=Find(HT,x,'c');
//		if(res)printf("%s\n",res);
//		else(printf("NULL\n"));
//	}
//}
