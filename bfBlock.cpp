#include"filesystem.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
using namespace std;
vector<int> balloc(int blockNum)
{
	vector<int>vec;
	if (filsys.s_fsize >= blockNum)
	{
		while (blockNum--)
		{
			if (filsys.s_nfree > 1)
			{
				filsys.s_nfree--;
				filsys.s_fsize--;
				vec.push_back(filsys.s_free[filsys.s_nfree]);
				filsys.s_free[filsys.s_nfree] = 0;
			}
			else
			{
				int bnum = filsys.s_free[--filsys.s_nfree];
				if (bnum == -1)
				{
					cout << "data disk is full!" << endl;
					return vec;
				}
				vec.push_back(bnum);
				filsys.s_fsize--;
				fseek(fd,DATASTART+bnum*BLOCKSIZ,SEEK_SET);
				fread(&filsys.s_free[0],sizeof(unsigned int)*NICFREE,1,fd);
				filsys.s_nfree = NICFREE;
			}
		}
		fseek(fd,BLOCKSIZ,SEEK_SET);
		fwrite(&filsys, sizeof(struct filsys), 1, fd);
		return vec;
	}
	else
	{
		cout << "data empty disk  block is enough!" << endl;
	}
}
unsigned int block_buf[BLOCKSIZ / sizeof(int)];
void freeBlock(vector<int>blockSet)
{
	for (int i = 0; i < blockSet.size(); i++)
	{
		int num = blockSet[i];
		filsys.s_nfree++;
		filsys.s_fsize++;
		if (filsys.s_nfree > NICFREE)
		{
			for (int i = 0; i < NICFREE; i++)
			{
				block_buf[i] = filsys.s_free[i];
			}
			fseek(fd,DATASTART+num*BLOCKSIZ,SEEK_SET);
			fwrite(block_buf,BLOCKSIZ,1,fd);
			for (int m = 0; m < 50; m++)
				filsys.s_free[m] = 0;
			filsys.s_nfree = 1;
			filsys.s_free[filsys.s_nfree - 1] = num;
		}
		else
		{
			filsys.s_free[filsys.s_nfree - 1] = num;
		}
	}
	fseek(fd,BLOCKSIZ,SEEK_SET);
	fwrite(&filsys,BLOCKSIZ,1,fd);
}
