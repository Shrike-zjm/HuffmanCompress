int WordNum=0;
int TotalWord=0;
struct AVLNode
{
	char* Word;
	int total;
	
	struct AVLNode* Left;
	struct AVLNode* Right;
	struct AVLNode* Parent;
	int level;
	int height;
};
typedef struct AVLNode* AVLPtr;
typedef AVLPtr AVLTree;

struct QNode
{
	AVLTree anPtr;
	struct QNode* Next;
};
typedef struct QNode* qnPtr;
//typedef qnPtr Queue;
struct queue
{
	qnPtr head;
	qnPtr tail;
};
typedef struct queue* Queue;

Queue Qcre()
{
	Queue q=malloc(sizeof(struct queue));
	q->head=q->tail=NULL;
	return q;
}

Queue Qadd(Queue theQ,AVLTree anPtr)//入队
{
	qnPtr qn=malloc(sizeof(struct QNode));
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

AVLTree Qdel(Queue theQ)//出队
{
	qnPtr head=theQ->head;
	theQ->head=head->Next;
	AVLTree a=head->anPtr;
	free(head);
	return a;
}
void printBetter(AVLTree avlt)//
{
	if(avlt)
	{
		AVLTree ap;
		Queue Q2=Qcre();
		Qadd(Q2,avlt);
		//printf("(Height:%d) ",avlt->Height);
		//int nowHeight=avlt->Height;
		int nowlevel=1;
		while(Q2->head!=NULL)
		{
			ap=Qdel(Q2);
			if(ap->level>nowlevel)
			{
				nowlevel++;
				printf("\n");
			}
			printf("%s ",ap->Word);
			printf("(%d) ",ap->total);
			//printf("(level:%d) ",ap->level);
			if(ap->Left)
			{
				//printf("(Height:%d) ",ap->Left->Height);
				Qadd(Q2,ap->Left);
			}
			if(ap->Right)
			{
//				if(ap->Right->Height<nowHeight)
//				{
//					printf("\n");
//					nowHeight--;
//				}
				//printf("(Height:%d) ",ap->Right->Height);
				Qadd(Q2,ap->Right);
			}

		}
		printf("\n");
	}
}



int GetHeight(AVLTree T)//在insert中设置初始高度 这里不赋高度
{
	if(T)
	{
		return 1+max(GetHeight(T->Left),GetHeight(T->Right));
	}
	else return 0;
}
void ChangeAllTheLevel(AVLTree T,int x)
{
	if(T)
	{
		T->level+=x;
		ChangeAllTheLevel(T->Left,x);
		ChangeAllTheLevel(T->Right,x);
	}
}
//调用的时候一定要 root's parent->left=xxxRotation()
AVLTree SingleLeftRotation(AVLTree A)
{
	AVLTree oldParent=A->Parent;
	AVLTree B=A->Left;
	A->Left=B->Right;
	B->Right=A;

	A->Parent=B;
	if(A->Left)A->Left->Parent=A;
	B->Parent=oldParent;
	
 	A->height=GetHeight(A);
	AVLTree ChangeHeight=B;
	while(ChangeHeight)
	{
		ChangeHeight->height=GetHeight(B);
		ChangeHeight=ChangeHeight->Parent;
	}
	
	A->level++;
	B->level--;
	ChangeAllTheLevel(A->Right,1);
	ChangeAllTheLevel(B->Left,-1);
	
	return B;
}
AVLTree SingleRightRotation(AVLTree A)
{
	AVLTree oldParent=A->Parent;

	AVLTree B=A->Right;
	A->Right=B->Left;
	B->Left=A;

	A->Parent=B;
	if(A->Right)A->Right->Parent=A;
	B->Parent=oldParent;

 	A->height=GetHeight(A);
	AVLTree ChangeHeight=B;
	while(ChangeHeight)
	{
		ChangeHeight->height=GetHeight(B);
		ChangeHeight=ChangeHeight->Parent;
	}
	A->level++;
	B->level--;
	ChangeAllTheLevel(A->Left,1);
	ChangeAllTheLevel(B->Right,-1);
	
	return B;
}

AVLTree DoubleLeftRightRotation(AVLTree A)
{
	A->Left=SingleRightRotation(A->Left);
	return SingleLeftRotation(A);
}
AVLTree DoubleRightLeftRotation(AVLTree A)
{
	A->Right=SingleLeftRotation(A->Right);
	return SingleRightRotation(A);
}
int Balance(AVLTree T)
{
	int diff=GetHeight(T->Left)-GetHeight(T->Right);
	if(diff>=-1&&diff<=1)return 0;
	else return diff;
}
AVLTree AVLInsert(AVLTree AVLT,char* data)//一定要有返回值 因为根会变
{
	AVLTree T=malloc(sizeof(struct AVLNode));
	T->Word=data;
	T->Left=T->Right=NULL;
	T->height=1;
	T->total=1;
	//还剩Parent和Level
	if(!AVLT)
	{
		WordNum++;
		T->level=1;
		T->Parent=NULL;
		return T;
	}
	else
	{
		int succeed=0;
		AVLTree AimT=AVLT;
		while(!succeed)//只管二叉搜索树的插入
		{
			if(strcmp(T->Word,AimT->Word)==0)
			{
				succeed=1;
				AimT->total++;
				free(T);
			}
			else if(strcmp(T->Word,AimT->Word)<0)
			{
				if(AimT->Left)
				{
					AimT=AimT->Left;
				}
				else
				{
					WordNum++;
					succeed=1;
					AimT->Left=T;
					T->Parent=AimT;
					//T->level=AimT->level++;
					T->level=AimT->level+1;
				}
			}
			else if(strcmp(T->Word,AimT->Word)>0)
			{
				if(AimT->Right)
				{
					AimT=AimT->Right;
				}
				else
				{
					WordNum++;
					succeed=1;
					AimT->Right=T;
					T->Parent=AimT;
					//T->level=AimT->level++;
					T->level=AimT->level+1;
				}
			}
  		}
		
		AVLTree NeedChange=AimT;
		while(NeedChange)//更新树高
		{
			NeedChange->height=GetHeight(NeedChange);
			NeedChange=NeedChange->Parent;
		}

		AVLTree JudgeRotation=AimT;
		int rotationSucceed=0;
		while(JudgeRotation&&!rotationSucceed)
		{
			char where='l';
			AVLTree Parent=JudgeRotation->Parent;
			if(Parent&&Parent->Right==JudgeRotation)where='r';
			int diff=Balance(JudgeRotation);
			if(diff==2)
			{
				char* Jdata=JudgeRotation->Left->Word;
				if(strcmp(data,Jdata)<0)JudgeRotation=SingleLeftRotation(JudgeRotation);
				else JudgeRotation=DoubleLeftRightRotation(JudgeRotation);
				rotationSucceed=1;
			}
			else if(diff==-2)
			{
				char* Jdata=JudgeRotation->Right->Word;
				if(strcmp(data,Jdata)>0)JudgeRotation=SingleRightRotation(JudgeRotation);
				else JudgeRotation=DoubleRightLeftRotation(JudgeRotation);
				rotationSucceed=1;
			}
			
			if(Parent)
			{
				if(where=='l') Parent->Left=JudgeRotation;
				else Parent->Right=JudgeRotation;
			}
			else
			{
				AVLT=JudgeRotation;
			}
			
			JudgeRotation=JudgeRotation->Parent;
		}
	}
	return AVLT;
}
void FreeAVLTree(AVLTree AVLT)
{
	if(AVLT)
	{
		AVLTree Left=AVLT->Left;
		AVLTree Right=AVLT->Right;
		free(AVLT->Word);
		free(AVLT);
		FreeAVLTree(Left);
		FreeAVLTree(Right);
	}
}
