#include"filesystem.h"
#include<stdio.h>
#include"stdlib.h"
void initHInode()
{
	for (int i = 0; i < NHINO; i++)
	{
		struct inode * pinode = (struct inode*)malloc(sizeof(struct inode));
		memset(pinode,0,sizeof(struct inode));
		pinode->i_forw = NULL;
		pinode->i_back = NULL;
		hinode[i].i_forw = pinode;
	}
}
