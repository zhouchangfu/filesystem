#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#include"filesystem.h"
using namespace std;
struct inode* iget(int dinode)//获取空闲i节点的块号
{
	struct inode*temp;
	int inodeid = dinode%NHINO;
	int existed = 0;
	temp = hinode[inodeid].i_forw;
	//cout <<"hinode[inodeid]"<< temp << endl;
	while (temp)
	{
		//cout << temp->i_ino << endl;
		if (temp->i_ino == dinode)
			/*existed */
		{
			existed = 1;
			break;
		}
		else    /*not existed */
			temp = temp->i_forw;
	};
	if (existed)
	{
		return temp;
	}
	if (DINODEBLK*BLOCKSIZ / DINODESIZ < dinode)
	{
		cout << "inode out of index of inode !" << endl;
	}
	struct inode * pinode = (struct inode *)malloc(sizeof(struct inode));
	memset(pinode,0,sizeof(struct inode));
	fseek(fd,DINODESTART+ DINODESIZ*dinode,SEEK_SET);
	fread(&pinode->di_number,sizeof(struct dinode),1,fd);
	pinode->i_back = NULL;
	pinode->i_forw = NULL;
	pinode->i_ino = dinode;
	if (hinode[inodeid].i_forw->i_forw == NULL)
	{
		hinode[inodeid].i_forw->i_forw = pinode;
		pinode->i_back = hinode[inodeid].i_forw;
	}
	else
	{
		hinode[inodeid].i_forw->i_forw->i_back = pinode;
		pinode->i_forw = hinode[inodeid].i_forw->i_forw;
		pinode->i_back = hinode[inodeid].i_forw;
		hinode[inodeid].i_forw->i_forw = pinode;

	}
	return pinode;
}
void iput(int dinode)
{
	int existed = false;
	int inodeid = dinode % NHINO;
	struct inode * temp;
	if (hinode[inodeid].i_forw == NULL)
	{
		//cout << "inode:" << dinode << " is not existed in memory!" << endl;
		return;
	}
	else
	{
		temp = hinode[inodeid].i_forw;
		//cout <<"hinode[inodeid]"<< temp << endl;
		while (temp)
		{
			//cout << temp->i_ino << endl;
			if (temp->i_ino == dinode)
				/*existed */
				{
				existed = 1;
					break;
				}
			else    /*not existed */
				temp = temp->i_forw;
		};
	}
	if (existed)
	{
		fseek(fd,DINODESTART+dinode*DINODESIZ,SEEK_SET);
		fwrite(&temp->di_number,sizeof(struct dinode),1,fd);
		return;
	}
	else
	{
		cout << "the inode:" << dinode << " is not exist in the hinode" << endl;
		return;
	}
}
void deleteInode(int dinode)
{
	struct inode*temp;
	int inodeid = dinode%NHINO;
	int existed = 0;
	temp = hinode[inodeid].i_forw;
	//cout <<"hinode[inodeid]"<< temp << endl;
	while (temp)
	{
		//cout << temp->i_ino << endl;
		if (temp->i_ino == dinode)
			/*existed */
		{
			existed = 1;
			break;
		}
		else    /*not existed */
			temp = temp->i_forw;
	};
	if (existed)
	{
		temp->i_back->i_forw = temp->i_forw;
		if(temp->i_forw!=NULL)
		temp->i_forw->i_back = temp->i_back;
		free(temp);
	}
	else
	{
		cout << "the file that you want to delete is not existed!!" << endl;
	}
}
