#include"filesystem.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
void install()
{
	/*if(fd==NULL)*/
	if(fd!=NULL)
    fclose(fd);
    fd = fopen("G:\\Qt_program\\filsystem\\filesystem.txt", "r+b");;
	memset(dir_buf,0,2*BLOCKSIZ);
	fseek(fd, DATASTART, SEEK_SET);
	fread(dir_buf,2*BLOCKSIZ,1,fd);
	showDir(cur_dir_num);
	
	memset(&filsys,0,sizeof(struct filsys));
	fseek(fd,BLOCKSIZ, SEEK_SET);
	fread(&filsys,BLOCKSIZ,1,fd);
	/*cout << sizeof(struct filsys) << endl;
	outputFilSys();*/

	memset(pwd, 0, PWDNUM*sizeof(struct pwd));
	fseek(fd,DATASTART+BLOCKSIZ*2, SEEK_SET);
	fread(&pwd, sizeof(struct pwd)*PWDNUM, 1, fd);
	
	showPassword();
}
void writeBack()
{
    if(fd!=NULL)
    fclose(fd);
    fd = fopen("G:\\Qt_program\\filsystem\\filesystem.txt", "r+b");;
    fseek(fd, DATASTART, SEEK_SET);
    fwrite(dir_buf,2*BLOCKSIZ,1,fd);

    fseek(fd,BLOCKSIZ, SEEK_SET);
    fwrite(&filsys,BLOCKSIZ,1,fd);

    fseek(fd,DATASTART+BLOCKSIZ*2, SEEK_SET);
    fwrite(&pwd, sizeof(struct pwd)*PWDNUM, 1, fd);


    fseek(fd,DATASTART+BLOCKSIZ*2, SEEK_SET);
    fwrite(&pwd, sizeof(struct pwd)*PWDNUM, 1, fd);
}
