#include"filesystem.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
void showDir(int upDirNum)
{
	cout <<"\t\t\t\t"<<"curDirectory:" << endl;
	for (int i = 0; i <2 * BLOCKSIZ / (DIRSIZ + 4); i++)
	{
		if (dir_buf[i].d_ino != 0)
		{
			struct inode* pinode = iget(dir_buf[i].d_ino);
			if(pinode->di_updir== upDirNum)
			cout << dir_buf[i].d_name << endl;
		}
	}
}
void showPassword()
{
	cout << "p_uid" << "\t" << "p_gid" << "\tpassword" << endl;
	for (int i = 0; i < PWDNUM; i++)
	{
		if (pwd[i].p_uid != 0)
		{
			cout << pwd[i].p_uid << "\t" << pwd[i].p_gid << "\t" << pwd[i].password << endl;
		}
	}
}
