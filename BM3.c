 #include <stdio.h>
 #include <string.h>
 // strong good suffix implementation
void LineAndPos(char* Article,int s,int* Lines,int* Pos,int* LastCharPos,int MaxCodeLength,HashTable CodeToWord)
{
	char* current01=malloc(sizeof(char)*(1+MaxCodeLength));
	int len=0;
	while(len+(*LastCharPos)<s)
	{
		len++;
		strncpy(current01,Article+*LastCharPos,len);
		current01[len]='\0';
		char* res=HashFind(CodeToWord,current01,'c');
		if(res)
		{
			*LastCharPos+=len;
			len=0;
			if(*res=='\n')
			{
				(*Lines)+=1;
				*Pos=1;
			}
			else
			{
				(*Pos)+=1;
			}
		}
	}
	free(current01);
}
void preprocess_strong_suffix(int* shift, int* bopos,char* pattern, int m)
{
     int i = m, j = m + 1; // length of pattern is m
     bopos[i] = j;
     while (i > 0) {
         while (j <= m && pattern[i - 1] != pattern[j - 1]) // if not euqal continue to find border
         {
             /*         Now no skipping the occurrences and shift the pattern         from i to j. */
             if (shift[j] == 0)
                 shift[j] = j - i; /* Character preceding the occurrence of T in
                                     pattern P is different than the mismatching character in P */
             j = bopos[j]; //Change the position of next border
         }
         i--;
         j--; /* store the beginning position of border when border found*/
         bopos[i] = j;
     }
 }
void preprocess_case2(int* shift, int* bopos,
     char* pat, int m) // case 2 Preprocessing
 {
     int i, j;
     j = bopos[0];
     for (i = 0; i <= m; i++) {
         if (shift[i] == 0)
             shift[i] = j;
         if (i == j) /* here the suffix became smaller than bopos[0] */
             j = bopos[j]; // j will be used as next wider border
     }
 }
 void good_suffix_algo(char* text, char* pattern,int* FoundTimes,int* Lines,int* Pos,int* LastCharPos,int MaxCodeLength,HashTable HashT) // Implement good suffix algo
 {
     int s = 0, j; // s is shift of pattern w.r.t text
     int m = strlen(pattern);
     int n = strlen(text);
     int bopos[m + 1], shift[m + 1];
     for (int i = 0; i < m + 1; i++)
         shift[i] = 0; // initially all shifts are 0
     preprocess_strong_suffix(shift, bopos, pattern, m); // preprocess the pattern
     preprocess_case2(shift, bopos, pattern, m); // case 2 Preprocessing
     while (s <= n - m) {
         j = m - 1;
         while (j >= 0 && pattern[j] == text[s + j]) /* j reduces while text and pattern characters keep matching at s shift*/
             j--;
         if (j < 0) // Pattern present at cuurent shift makes j < 0
         {
             //printf("Pattern found at shift = %d\n", s);
             printf("二进制文件中，出现位置的下标是 %d    ", s);
             LineAndPos(text,s,Lines,Pos,LastCharPos,MaxCodeLength,HashT);
             printf("行数是 %d    列数是 %d\n",*Lines,*Pos);
             s += shift[0];
             (*FoundTimes)++;
         }
         else
             /*pattern[i] != pattern[s+j] the pattern shifts j+1 times */
             s += shift[j + 1];
     }
 }
