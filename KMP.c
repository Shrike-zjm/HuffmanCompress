#define ERROR -271828
int GetLen(char* str)
{
	int len=0;
	while(*str!='\0')
	{
		len++;
		str++;
	}
	return len;
}
int* GetNext(char* str)
{
	int len=GetLen(str);
	int* NextValue=malloc(sizeof(int)*(len+1));//0号不用
	NextValue[1]=0;
	if(len==1)return NextValue;
	NextValue[2]=1;
	int j;
	for(j=2;j<=len-1;j++)
	{
		int k=NextValue[j];
		int strj=j-1;
		int strk=k-1;
		if(str[strj]==str[strk])
		{
			NextValue[j+1]=NextValue[j]+1;
		}
		else
		{
			while(1)
			{
				k=NextValue[k];
				strk=k-1;
				if(str[strj]==str[strk])
				{
					NextValue[j+1]=k+1;
					break;
				}
				if(k==0)
				{
					NextValue[j+1]=1;
					break;
				}
			}
		}
	}
	return NextValue;
}
int* DirectNext(char* str)
{
	int len=GetLen(str);
	int* NextValue=malloc(sizeof(int)*(len+1));//0号不用
	int i;
	for(i=1;i<=len;i++)
	{

	}
}
int KMPFind(char* txt,char* pat)
{
	int* array=GetNext(pat);
	int i=0;
	int j=0;
	while(i<=GetLen(txt)-1)
	{
		//if(txt[i]=pat[j])
		if(txt[i]==pat[j])
		{
			if(j==GetLen(pat)-1)
			{
				return(i-j);
			}
			i++;
			j++;
		}
		else
		{
			j=array[j+1]-1;
		}
		if(j==-1)
		{
			j=0;
			i++;
		}
	}
	return ERROR;
}
