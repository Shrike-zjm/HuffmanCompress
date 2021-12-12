#include<stdio.h>
#include<string.h>
typedef unsigned char* byte_pointer;

void show_bytes(byte_pointer start,size_t len)
{
	//size_t i;
	int first=1;
	int i;
	for(i=0;i<=len-1;i++)
	{
		if(first)
		{
			printf("%.2x",start[i]);
			first=0;
		}
		else printf(" %.2x",start[i]);
	}
	printf("\n");
}
void show_str(char x[])
{
	printf("size if %d\n",sizeof(x));
	show_bytes((byte_pointer)x,sizeof(x));
}
void show_int(int x)
{
	show_bytes((byte_pointer)&x,sizeof(int));
}

void show_float(float x)
{
	show_bytes((byte_pointer)&x,sizeof(float));
}

void show_pointer(void* x)
{
	show_bytes((byte_pointer)&x,sizeof(void *));
}


