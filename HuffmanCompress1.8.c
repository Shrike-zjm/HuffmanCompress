#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "sconv.c"
int UTF8;
#include"max.c"
#include"MinHeap.c"
#include"HashTable.c"
#include"BM3.c"
#define JudgeEncodeNumber 128

int isBeginWith10(unsigned char uc)
{
	if(uc>=0x80&&uc<0xc0)return 1;
	else return 0;
}
int isUTF_8(FILE* fp)
{
	int res=1;
	int num=0;
	char currentChar=fgetc(fp);
	while((currentChar!=EOF)&&(num<JudgeEncodeNumber))
	{
		int judgeTimes,i;
		if(currentChar>=0)judgeTimes=0;
		else
		{
			unsigned char uchar=(unsigned)currentChar;
			if(uchar>=0xfc&&uchar<0xfe)judgeTimes=5;
			else if(uchar>=0xf8)judgeTimes=4;
			else if(uchar>=0xf0)judgeTimes=3;
			else if(uchar>=0xe0)judgeTimes=2;
			else if(uchar>=0xc0)judgeTimes=1;
			else return 0;
		}
		for(i=1;i<=judgeTimes;i++)
		{
			currentChar=fgetc(fp);
			if(!isBeginWith10(currentChar))return 0;
		}
		currentChar=fgetc(fp);
		num++;
	}
	return res;
}
char GetChineseChar(FILE* fp,char currentChar,char** dest)//+1在insert函数里解决
{
	TotalWord++;
	char* symbol;
	if(UTF8==1)
	{
		int SizeOfChinese;
		if(currentChar>=0)SizeOfChinese=1;
		else
		{
			unsigned char uchar=(unsigned)currentChar;
			if(uchar>=0xfc&&uchar<0xfe)SizeOfChinese=6;
			else if(uchar>=0xf8)SizeOfChinese=5;
			else if(uchar>=0xf0)SizeOfChinese=4;
			else if(uchar>=0xe0)SizeOfChinese=3;
			else if(uchar>=0xc0)SizeOfChinese=2;
		}
		symbol=malloc(sizeof(char)*(SizeOfChinese+1));
		symbol[0]=currentChar;
		int i;
		for(i=1;i<=SizeOfChinese-1;i++)
		{
			symbol[i]=fgetc(fp);
		}
		symbol[SizeOfChinese]='\0';
		//OriginBytes+=SizeOfChinese;
	}
	else//GBK-2312字符读入
	{
		int SizeOfChar,i;
		if(currentChar&0x80)SizeOfChar=2;
		else SizeOfChar=1;//是英文字符
		symbol=malloc(sizeof(char)*(SizeOfChar+1));
		symbol[0]=currentChar;
		for(i=1;i<=SizeOfChar-1;i++)
		{
			symbol[i]=fgetc(fp);
		}
		symbol[SizeOfChar]='\0';
	}
	*dest=symbol;
	char realCurrentChar=fgetc(fp);
	return realCurrentChar;
}
int IntoBigFlow(HashTable HT,char* BigFlow,char* FileName,char type)
{
	FILE* fp=fopen(FileName,"r");
	char currentChar=fgetc(fp);
	int current=0;
	while(currentChar!=EOF)
	{
		char* dest;
		currentChar=GetChineseChar(fp,currentChar,&dest);

		char* Code=HashFind(HT,dest,type);
		strcpy(BigFlow+current,Code);
		//*(BigFlow+current+strlen(Code))='\0';
		current+=strlen(Code);
		free(dest);
	}
	fclose(fp);
	return current;
}
//前四个东西是int 依次为 是否UTF-8 合适的质数 压缩文本的总比特数 码表的字节数
void HuffmanWrite(char* WriteName,char* ArticleBigFlow,char* CodeTableFlow,int CTFLen,int BigFlowBits,int thePrime)
{
	//FILE* fp=fopen(WriteName,"w+"); 不能这么写！
	FILE* fp=fopen(WriteName,"w+b");
	fwrite(&UTF8,4,1,fp);
	fwrite(&thePrime,4,1,fp);
 	fwrite(&BigFlowBits,4,1,fp);
	fwrite(&CTFLen,4,1,fp);
	int i=0;
	for(i=0;i<=CTFLen-1;i++)
	{
		fwrite(CodeTableFlow+i,1,1,fp);
	}
	int current=0;
	char* ptr=ArticleBigFlow;
	char* byte=malloc(sizeof(char)*9);
	//while(*(ptr+current)!='\0')//最后一字节可能会有问题
	while(current<BigFlowBits)
	{
		strncpy(byte,ArticleBigFlow+current,8);
		byte[8]='\0';
		//printf("%s ",byte);
		unsigned the8bits=CodeToInt8(byte);
		//printf("%.2x\n",the8bits);
		fwrite(&the8bits,1,1,fp);
		current+=8;
	}
	free(byte);
	free(ArticleBigFlow);

	fclose(fp);
}

//void IntoCTF(HuffmanTree HT,char* CodeTableFlow,int current)
void IntoCTF(HuffmanTree HT,char* CodeTableFlow,int* current)
{
	if(HT)
	{
		if(HT->Word)//是叶结点
		{
			//注意 哈夫曼编码一旦大于31位就会出大问题 这一个字节的节省是有代价的！
			char CharBytes=strlen(HT->Word);
			char CodeBits=strlen(HT->Code);
			char LenInfo=(CharBytes<<5)|(CodeBits);
			*(CodeTableFlow+(*current))=LenInfo;
			(*current)+=1;

			strcpy((CodeTableFlow+(*current)),HT->Word);
			(*current)+=strlen(HT->Word);

			int times=1+CodeBits/8;//这里的问题 只有1-7需要+1
			if(CodeBits%8==0)times-=1;
			char* ptr=HT->Code;
			char* byte=malloc(sizeof(char)*9);
			int i;
			for(i=0;i<=times-1;i++)
			{
				strncpy(byte,ptr+i*8,8);
				byte[8]='\0';
				char the8bits=CodeToInt8(byte);
				*(CodeTableFlow+(*current))=the8bits;
				(*current)+=1;
			}
			*(CodeTableFlow+(*current))='\0';
			free(byte);
		}

		IntoCTF(HT->Left,CodeTableFlow,current);
		IntoCTF(HT->Right,CodeTableFlow,current);

	}
}
char* PatternTo01(char* Pattern,int MaxCodeLength,char ArticleEncoding,HashTable HashT)
{
	//windows命令行的编码就是GBK 只能先读再转
//	if(*Pattern='\0')return NULL;
	if(*Pattern=='\0')return NULL;
	int current=0;
	char* ZeroAndOne=malloc(sizeof(char)*(strlen(Pattern)*MaxCodeLength+1));
	if(ArticleEncoding=='g')
	{
		while((*Pattern)!='\0')
		{
			char* symbol;
			int SizeOfChar=1;
			if((*Pattern)&0x80)SizeOfChar=2;
			symbol=malloc(sizeof(char)*(SizeOfChar+1));
			symbol[0]=*Pattern;
			if(SizeOfChar==2)
			{
				Pattern++;
				symbol[1]=*Pattern;
			}
			symbol[SizeOfChar]='\0';
			Pattern++;
			char* CodeOfSymbol=HashFind(HashT,symbol,'g');
			free(symbol);
			if(CodeOfSymbol)strcpy(ZeroAndOne+current,CodeOfSymbol);
			else return NULL;
			current=strlen(ZeroAndOne);
		}
	}
	else if(ArticleEncoding=='u')
	{
		while((*Pattern)!='\0')
		{
			char* symbol;
			int SizeOfChinese;
			if(*Pattern>=0)SizeOfChinese=1;
			else
			{
				unsigned char uchar=(unsigned)(*Pattern);
				if(uchar>=0xfc&&uchar<0xfe)SizeOfChinese=6;
				else if(uchar>=0xf8)SizeOfChinese=5;
				else if(uchar>=0xf0)SizeOfChinese=4;
				else if(uchar>=0xe0)SizeOfChinese=3;
				else if(uchar>=0xc0)SizeOfChinese=2;
			}
			symbol=malloc(sizeof(char)*(SizeOfChinese+1));
			symbol[0]=*Pattern;
			Pattern++;
			int i;
			for(i=1;i<=SizeOfChinese-1;i++)
			{
				symbol[i]=*Pattern;
				Pattern++;
			}
			symbol[SizeOfChinese]='\0';
			char* CodeOfSymbol=HashFind(HashT,symbol,'u');
			free(symbol);
			if(CodeOfSymbol)strcpy(ZeroAndOne+current,CodeOfSymbol);
			else return NULL;
			current=strlen(ZeroAndOne);
		}
		
	}
	return ZeroAndOne;
}
char* ToUTF8(char* str)
{
	int size=sconv_gbk_to_unicode(str, -1, NULL, 0);
	wchar *unicode_str = malloc(sizeof(char)*(size+4));
	size = sconv_gbk_to_unicode(str, -1, unicode_str, size);
    unicode_str[size / 2] = 0;
    size = sconv_unicode_to_utf8(unicode_str, -1, NULL, 0);
    char *utf8_str = malloc(sizeof(char)*(size+1));
    sconv_unicode_to_utf8(unicode_str, -1, utf8_str, size);
    utf8_str[size] = 0;
    free(unicode_str);
    return utf8_str;
}
int main()
{
	char* command=malloc(sizeof(char)*101);
	while(1)
	{
		printf("————————————————————————————\n请输入操作命令:\n");
		printf("【c】:压缩文件\t【u】:解压文件\t【f】:在压缩文件中查找\t【q】:退出程序\n");
		scanf("%s",command);
		switch(command[0])
		{
			case 'c':
			{
				WordNum=0;
				TotalWord=0;
				clock_t start,stop;
				printf("当前目录下待处理的文件名(带扩展名)为:\n");
				char* FileName=malloc(sizeof(char)*121);
				getchar();
				gets(FileName);
				start=clock();
				FILE* fp=fopen(FileName,"r");
				if(fp)
				{
					FILE* fp2=fopen(FileName,"r");
					if(isUTF_8(fp2))
					{
						printf("The file encoding is UTF-8.\n");
						UTF8=1;
					}
					else
					{
						printf("The file encoding is GBK-2312(maybe).\n");
						UTF8=0;
					}
					fclose(fp2);
					char CodeType='g';
					if(UTF8)CodeType='u';
					//以上部分为判断中文编码


					clock_t startAVL,stopAVL;
					startAVL=clock();
					FHTable FreHT=CreFHTable(NextPrime(6000));
					char currentChar=fgetc(fp);
					int OriginBytes=0;
					while(currentChar!=EOF)
					{
						char* dest;
						currentChar=GetChineseChar(fp,currentChar,&dest);
						OriginBytes+=strlen(dest);
						FHInsert(FreHT,dest);
					}
					fclose(fp);
					stopAVL=clock();


					//以上部分将字频统计进哈希表FreHT中
					//接下来把FreHT的结点放到最小堆中
					MinHeap MH=CreateMinHeap(WordNum);
					FHTIntoHeap(FreHT,MH);

					//哈希表优化读取频数的目的已达到，结束使命。
					FreeFHTable(FreHT);

					//创建哈夫曼树 并根据哈夫曼树给每个结点赋上编码
					HuffmanTree HT=HuffmanCre(MH);
                    FreeMinHeap(MH);

					SetCode(HT,'f',0,NULL);
					//if(command[1]=='c')printHuffmanBetter(HT);


					int MaxCodeLength=GetHuffmanHeight(HT)-1;
					printf("\nHEIGHT is:%d\n",GetHuffmanHeight(HT));
					printf("WordNum is:%d\n",WordNum);
					int thePrime=NextPrime(2*WordNum);
					printf("ThePrime is:%d\n",thePrime);
					printf("TotalWord is:%d\n",TotalWord);

					//创建由字到哈夫曼码的哈希表
					HashTable WordToCode=CreHashTable(thePrime);
					InsertToHash(HT,WordToCode,CodeType);
					//Word和Code还储存在哈夫曼树的节点里 还不能Free


					//BigFlow是一个极大的char数组
					//先把要写的二进制文件每八位以一个char的形式存到BigFlow里
					int i;
					char* BigFlow=malloc(sizeof(char)*MaxCodeLength*TotalWord+8);
					int ArticleBits=IntoBigFlow(WordToCode,BigFlow,FileName,CodeType);
					printf("ArticleBits is: %d\n",ArticleBits);
					for(i=1;i<=7;i++)//草
					{
						*(BigFlow+i+strlen(BigFlow))='\0';
					}


					//同理 把码表存到CodeTableFlow里
					int MaxCharLength=2;
					if(UTF8)MaxCharLength=6;
					char* CodeTableFlow=malloc(sizeof(char)*WordNum*(1+MaxCharLength+1+MaxCodeLength/8));//定个上限就行(最后直接写它)
					int current=0;
					IntoCTF(HT,CodeTableFlow,&current);
					printf("The length of CodeTableFlow is : %d\n",current);
					
					
					//最后把这两个Flow写到文件里
					int ZipBytes=4*4+current+ArticleBits/8+(ArticleBits/8!=0);

					int FNLen=strlen(FileName);
					char* FileNameCPY=malloc(sizeof(char)*121);
					strcpy(FileNameCPY,FileName);
					char after[20]="-Compressed";
					strcpy(FileName+FNLen,after);
					HuffmanWrite(FileName,BigFlow,CodeTableFlow,current,ArticleBits,thePrime);
					free(CodeTableFlow);

					stop=clock();
					printf("Compressed file successfully!\n");
					printf("Original file size is %d bytes.\n",OriginBytes);
					printf("Compressed file size is %d bytes.\n",ZipBytes);
					printf("The compression ratio is %.4f%%\n",100.0*ZipBytes/OriginBytes);
					double duration=((double)stop-start)/1000;
					double durationAVL=((double)stopAVL-startAVL)/1000;
					printf("It took %.2f second to create the FrequencyHashTable\n",durationAVL);
					printf("It took %.2f second to compress the ",duration);
					printf("%s\n",FileNameCPY);
					FreeHashTable(WordToCode);
					FreeHuffmanTree(HT);
					free(FileName);
					free(FileNameCPY);
				}
				else
				{
					printf("\nERROR:CANNOT FIND FILE!\n");
				}
				break;
			}
			case 'u':
			case 'f':
			{
				int MaxCodeLength=0;
				if(command[0]=='u')printf("待解压的文件名(“-Compressed”之前的部分)为:\n");
				else printf("待查找的压缩文件名(“-Compressed”之前的部分)为:\n");
				char* FileName=malloc(sizeof(char)*121);
				char* FileName2=malloc(sizeof(char)*121);
				getchar();
				gets(FileName);
				char after[20]="-Compressed";
				char after2[20]="Unzipped-";
				int FNLen=strlen(FileName);
				strcpy(FileName2,after2);
				strcpy(FileName2+9,FileName);
				strcpy(FileName+FNLen,after);
				FILE* fp=fopen(FileName,"rb");
				free(FileName);
				if(fp)
				{
					FILE* fp2;
					if(command[0]=='u')fp2=fopen(FileName2,"w+b");
					free(FileName2);
					//先读出文件最开始存的四个int
	 				int* isUTF8Ptr=malloc(sizeof(int));
					int* thePrimePtr=malloc(sizeof(int));
					int* ArticleBitsPtr=malloc(sizeof(int));;
					int* CodeTableBytesPtr=malloc(sizeof(int));;
					fread(isUTF8Ptr,4,1,fp);
					fread(thePrimePtr,4,1,fp);
					fread(ArticleBitsPtr,4,1,fp);
					fread(CodeTableBytesPtr,4,1,fp);
					UTF8=*isUTF8Ptr;
					char CodeType='g';
					if(UTF8)CodeType='u';
					HashTable CodeToWord=CreHashTable(*thePrimePtr);
					HashTable WordToCode;
					if(command[0]=='f')WordToCode=CreHashTable(*thePrimePtr);
					//u 解压 哈希表是码到字
					//f 查找 哈希表是字到码

					//接下来 先把码表读出来
					int PassedBytes=0;
					while(PassedBytes<*CodeTableBytesPtr)
					{

						//char LenInfo=fgetc(fp);
						char* LenInfoPtr=malloc(sizeof(char));
						fread(LenInfoPtr,1,1,fp);
						char LenInfo=*LenInfoPtr;
						//unsigned uLenInfo=LenInfo;
						PassedBytes++;
						int CharBytes=((unsigned)(LenInfo&0xe0))>>5;
						int CodeBits=LenInfo&0x1f;
						char* Word=malloc(sizeof(char)*(1+CharBytes));
						char* Code=malloc(sizeof(char)*(1+CodeBits));
						int i;
						for(i=0;i<=CharBytes-1;i++)
						{
							//Word[i]=fgetc(fp);
							fread(Word+i,1,1,fp);
							PassedBytes++;
						}
						Word[i]='\0';

						//unsigned currentByte;
						unsigned char* cBPtr=malloc(sizeof(char));
						unsigned char currentByte;
						int weight;
						for(i=0;i<=CodeBits-1;i++)
						{
							if(i%8==0)
							{
								fread(cBPtr,1,1,fp);
								currentByte=*cBPtr;
								//currentByte=fgetc(fp);
								PassedBytes++;
								weight=128;
							}
							char* thisBit=malloc(sizeof(char)*2);
							int thisBitNum=currentByte/weight;
							thisBit[0]='0'+thisBitNum;
							thisBit[1]='\0';//草草草草草
							strcpy(Code+i,thisBit);
							currentByte-=weight*thisBitNum;
							weight/=2;
							free(thisBit);
						}
						Code[i]='\0';
						
						if(command[0]=='f')
						{
							HashInsert(WordToCode,Code,Word,CodeType);
							if(strlen(Code)>MaxCodeLength)MaxCodeLength=strlen(Code);
						}
						HashInsert(CodeToWord,Code,Word,'c');
					}


					//码表读完了之后 再根据需求把文章读出来
					unsigned currentByte;
					char* Code;
					char* Article;
					if(command[0]=='u')Code=malloc(sizeof(char)*(65));
					else Article=malloc(sizeof(char)*((*ArticleBitsPtr)+8));
					int currentCodePos=0;
					int weight;
					int i;
					for(i=0;i<=*ArticleBitsPtr-1;i++)
					{
						if(i%8==0)
						{
							currentByte=fgetc(fp);
							weight=128;
						}
						char* thisBit=malloc(sizeof(char)*2);
						int thisBitNum=currentByte/weight;
						thisBit[0]='0'+thisBitNum;
						thisBit[1]='\0';//草草草草草

						if(command[0]=='u')
						{
							strcpy(Code+currentCodePos,thisBit);
							currentCodePos++;
							char* FoundWord=HashFind(CodeToWord,Code,'c');
							if(FoundWord)
							{
								currentCodePos=0;
								fputs(FoundWord,fp2);
							}
						}
						else strcpy(Article+i,thisBit);

						currentByte-=weight*thisBitNum;
						weight/=2;
						free(thisBit);
					}
					
					fclose(fp);


					if(command[0]=='u')
					{
						fclose(fp2);
						
						printf("\n——Unzip the file successfully!——\n\n");
					}
					else if(command[0]=='f')
					{
						//printf("%s",Article);
						printf("\n——Read the file successfully!——\n\n");
						char* Pattern=malloc(sizeof(char)*201);
						while(1)
						{
							printf("请输入要查找的内容:(输入“Quit!”退出查找)\n");
							gets(Pattern);
							if(strcmp(Pattern,"Quit!")==0)break;
							char* Pattern2;
							char* ZeroAndOnePattern;
							
							if(UTF8)
							{
								Pattern2=ToUTF8(Pattern);
								ZeroAndOnePattern=PatternTo01(Pattern2,MaxCodeLength,CodeType,WordToCode);
							}
							else ZeroAndOnePattern=PatternTo01(Pattern,MaxCodeLength,CodeType,WordToCode);
							if(!ZeroAndOnePattern)
							{
								printf("输入的字有一个根本就不在这篇文章里啊(╯°Д°)╯︵ ┻━┻\n");
								continue;
							}
							//printf("%s\n",ZeroAndOnePattern);

							int FoundTimes=0;
							int LastCharPos=0;
							int line=1;
							int pos=1;
//							LineAndPos(Article,5,&line,&pos,&LastCharPos,MaxCodeLength,CodeToWord);
//							printf("line is %d\npos is %d\n",line,pos);
//							LineAndPos(Article,17,&line,&pos,&LastCharPos,MaxCodeLength,CodeToWord);
//							printf("line is %d\npos is %d\n",line,pos);
							good_suffix_algo(Article,ZeroAndOnePattern,&FoundTimes,&line,&pos,&LastCharPos,MaxCodeLength,CodeToWord);
							if(FoundTimes==0)printf("没有找到“%s”！\n",Pattern);
							else printf("“%s”共出现了%d次。\n",Pattern,FoundTimes);
						}
						FreeHashTable(WordToCode);
						free(Pattern);
					}
					FreeHashTable(CodeToWord);
				}
				else
				{
					printf("\nERROR:CANNOT FIND FILE!\n\n");
				}
				break;
			}
			case 'q':
			{
				exit(0);
			}
			default:
			{
				printf("\n请输入正确的命令！\n");
			}
		}
	}

	free(command);
	return 0;
}
