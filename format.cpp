#include"filesystem.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
void format()
{


	unsigned int block_buf[BLOCKSIZ / sizeof(int)];
	char *buf;
	int i, j;
    if(fd!=NULL)
    fclose(fd);
    memset(dir_buf,0,2*BLOCKSIZ);
    memset(&filsys,0,sizeof(struct filsys));
    memset(pwd, 0, PWDNUM*sizeof(struct pwd));
    initHInode();
    remove("G:\\Qt_program\\filsystem\\filesystem.txt");
    fd = fopen("G:\\Qt_program\\filsystem\\filesystem.txt", "w+b");
	buf = (char *)calloc(1,(DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char));
	if (buf == NULL)
	{
		printf("\nfile system file creat failed! \n");
		exit(0);
	}
	memset(buf,0, (DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char));
	fseek(fd, 0, SEEK_SET);
	fwrite(buf, (DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char), 1, fd);
	//block 0 is for main directory!
	struct inode * p0 = iget(1);//0号索引节点没有用，1，2，3分别对应于main,dir1,dir2
	memset(p0,0,sizeof(struct inode));
	p0->i_flag = 'd';
	p0->di_size = 0;
	p0->i_ino = 1;
	p0->di_number = 0;
	p0->di_updir = ROOTDIR;
	iput(1);

	struct inode * p1 = iget(2);//0号索引节点没有用，1，2，3分别对应于main,dir1,dir2
	memset(p1, 0, sizeof(struct inode));
	p1->i_flag = 'd';
	p1->di_size = 0;
	p1->i_ino = 2;
	p1->di_number = 0;
	p1->di_updir = ROOTDIR;
	iput(2);

	struct inode * p2 = iget(3);//0号索引节点没有用，1，2，3分别对应于main,dir1,dir2
	memset(p2, 0, sizeof(struct inode));
	p2->i_flag = 'd';
	p2->i_ino = 3;
	p2->di_size = 0;
	p2->di_number = 0;
	p2->di_updir = ROOTDIR;
	iput(3);



	strcpy(dir_buf[1].d_name,"main");
	strcpy(dir_buf[2].d_name,"dir1");
	strcpy(dir_buf[3].d_name,"dir2");
	strcpy(dir_buf[4].d_name, "password.txt");
	dir_buf[1].d_ino = 1;
	dir_buf[2].d_ino = 2;
	dir_buf[3].d_ino = 3;
    dir_buf[4].d_ino = 0;
    strcpy(dir_buf[5].d_name, "\0\0\0\0");

	fseek(fd,DATASTART,SEEK_SET);
	fwrite(dir_buf,2*BLOCKSIZ,1,fd);

	//second ,third directory or file is in first block!
	//password.txt is in second block!
	struct  inode* p3 = iget(4);
	memset(p3, 0, sizeof(struct inode));
	p3->i_flag = 'f';
	p3->di_size = 0;
	p3->di_number =1;
	p3->i_ino = 4;
	p3->di_addr[0] = 2;
	p3->di_updir = ROOTDIR;;
	iput(4);

	pwd[0].p_uid = 2116; pwd[0].p_gid = 03;
	strcpy(pwd[0].password, "dddd");
	pwd[1].p_uid = 2117; pwd[1].p_gid = 03;
	strcpy(pwd[1].password, "bbbb");
	pwd[2].p_uid = 2118; pwd[2].p_gid = 04;
	strcpy(pwd[2].password, "abcd");
	pwd[3].p_uid = 2119; pwd[3].p_gid = 04;
	strcpy(pwd[3].password, "cccc");
	pwd[4].p_uid = 2220; pwd[4].p_gid = 05;
	strcpy(pwd[4].password, "eeee");

	fseek(fd, DATASTART+2*BLOCKSIZ, SEEK_SET);
	fwrite(pwd, BLOCKSIZ, 1, fd);

	filsys.s_fmod = 'S';
	filsys.s_isize = NICFREE-5;
	filsys.s_fsize = FILEBLK-3;

	for (int i = 0; i < NICFREE - 5;i++)
	{
		filsys.s_inode[i] = i + 5;
	}
	filsys.s_ninode = NICFREE - 5;
	block_buf[NICFREE - 1] = -1;
	for (i = 0; i<NICFREE - 1; i++)
		block_buf[NICFREE - 2 - i] = FILEBLK - i - 1;//存储空闲块号码，最后一个为终止空闲块
	fseek(fd, DATASTART + BLOCKSIZ * (FILEBLK - NICFREE), SEEK_SET);
	fwrite(block_buf, 1, BLOCKSIZ, fd);
	for (i = FILEBLK - NICFREE; i>2; i -= NICFREE)//空闲堆栈(成组链接法)
	{
		if (i - 2 < NICFREE)
			break;
		for (j = 0; j<NICFREE; j++)
		{
			block_buf[j] = i - j;
		}
		block_buf[50] = 50;
		fseek(fd, DATASTART + BLOCKSIZ * (i - NICFREE), SEEK_SET);
		fwrite(block_buf, 1, BLOCKSIZ, fd);
	}
	j = 0;
	for (i = i; i>2; i--)
	{
		filsys.s_free[j] = i;//空闲堆栈
		++j;
	}
	filsys.s_nfree = j;
	fseek(fd, BLOCKSIZ, SEEK_SET);//超级块
	fwrite(&filsys, 1, sizeof(struct filsys), fd);

	/*fd = fopen("filesystem.txt", "r+");;
	memset(&filsys, 0, sizeof(struct filsys));
	fseek(fd, BLOCKSIZ, SEEK_SET);
	fread(&filsys.s_isize, sizeof(struct filsys), 1, fd);
	outputFilSys();*/
//    mkdir("2116",ROOTDIR);
//    mkdir("2117",ROOTDIR);
//    mkdir("2118",ROOTDIR);
//    mkdir("2119",ROOTDIR);
//    mkdir("2220",ROOTDIR);
    fclose(fd);
    fd=NULL;

}
