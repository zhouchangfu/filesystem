#include"filesystem.h"
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include<vector>
#include<queue>
#include<qDebug>
using namespace std;
void mkdir(string dirname,int upDirNum)//创建目录
{
		int i;
		int existed = 0;
		for ( i= 1; i < 2*BLOCKSIZ / (DIRSIZ + 4); i++)
		{
			if (strcmp(dir_buf[i].d_name, "") == 0)
			{

					existed = 1;
					break;
			}
		}
        //查找空闲的目录块
		if (existed)//存在空间可以创建目录
		{
            if (filsys.s_ninode > 0)//如果存在空闲节点块
			{
                //*****************************test*******


				int inodeNum = filsys.s_inode[filsys.s_ninode-1];
				filsys.s_inode[filsys.s_ninode - 1] = 0;
				filsys.s_ninode--;
                filsys.s_isize--;//空闲节点块数减减
				struct inode* pinode = iget(inodeNum);
				for (int m = 0; m < NADDR; m++)
				{
					pinode->di_addr[m] = 0;
					pinode->sub_dir_no[m] = 0;
                }//空闲节点的子目录和占用块数清零
                pinode->i_ino = inodeNum;//记录i节点磁盘块号
                pinode->i_flag = 'd';//标识一个文件的类型为目录
                pinode->di_size = 0;//目录的大小为0
                pinode->di_number = 0;//子目录数目为0
                pinode->di_updir = upDirNum;//上级目录号码
                pinode->di_mode=ACCESSSREADANDWRITE;//设置读写访问权限
                pinode->di_uid=curUser.u_uid;//设置I节点id号
                strcpy(dir_buf[i].d_name,dirname.c_str());//设置目录名称
                dir_buf[i].d_ino = inodeNum;//设置i节点块号
                iput(inodeNum);//将i节点写回磁盘

				//写回main目录块
				fseek(fd,DATASTART,SEEK_SET);
				fwrite(dir_buf,2*BLOCKSIZ,1,fd);
				showDir(upDirNum);

				//写会超级块
				fseek(fd,BLOCKSIZ,SEEK_SET);
				fwrite(&filsys,sizeof(struct filsys),1,fd);

				int subDirNotFull = 0;
                if (upDirNum != ROOTDIR)//判断当前目录不为根目录的话
				{
					struct inode* pinode = iget(dir_buf[upDirNum].d_ino);
					for (int k = 0; k < NADDR; k++)
					{
						if (pinode->sub_dir_no[k] == 0)
						{
							pinode->sub_dir_no[k] = i;
							subDirNotFull = 1;
							iput(dir_buf[upDirNum].d_ino);
							break;
						}
                    }//将上级目录的子目录增加一个元素
					if (!subDirNotFull)
					{
						cout << "dir " << dir_buf[upDirNum].d_name << " is full!" << endl;
					}
				}     
				
			}
			else
			{
				cout << "disk inode space is full !" << endl;
			}
		}
		else
		{
			cout << "dir space is full !"<<endl;
		}

}
void mkfile(string filename,int blockNum,int upDirNum)//分配blockNum块的名字为filename的文件
{
		int i;
		int existed = 0;
		for (i = 1; i < 2*BLOCKSIZ / (DIRSIZ + 4); i++)
		{
			if (strcmp(dir_buf[i].d_name, "") == 0)
			{
					existed = 1;
					break;

			}
		}
		if (existed)//存在索引节点空间可以创建目录
		{
			if (filsys.s_ninode > 0)
			{
				if (filsys.s_fsize >= blockNum)//存在数据块可以分配来创建文件
				{
					vector<int>blockNoSet = balloc(blockNum);
					int inodeNum = filsys.s_inode[filsys.s_ninode - 1];
					filsys.s_inode[filsys.s_ninode - 1] = 0;
					filsys.s_ninode--;
					filsys.s_isize--;
					struct inode* pinode = iget(inodeNum);
					for (int m = 0; m < NADDR; m++)
					{
						pinode->di_addr[m] = 0;
						pinode->sub_dir_no[m] = 0;
					}
					pinode->i_flag = 'f';
					pinode->di_size = 0;
					pinode->di_number = 0;
					pinode->i_ino = inodeNum;
					pinode->di_updir = upDirNum;
                    pinode->di_mode=ACCESSSREADANDWRITE;
                    pinode->di_uid=curUser.u_uid;
					strcpy(dir_buf[i].d_name, filename.c_str());
					dir_buf[i].d_ino = inodeNum;
					int newi = i;
					for (int i = 0; i < blockNoSet.size(); i++)
					{
						pinode->di_addr[i] = blockNoSet[i];
					}
					//写回i节点
					iput(inodeNum);
					//写回子目录块
					fseek(fd, DATASTART, SEEK_SET);
					fwrite(dir_buf,2*BLOCKSIZ, 1, fd);
					showDir(upDirNum);

					//写会超级块
					fseek(fd, BLOCKSIZ, SEEK_SET);
					fwrite(&filsys, sizeof(struct filsys), 1, fd);
					int subDirNotFull = 0;
					if (upDirNum != ROOTDIR)
					{
						struct inode* pinode = iget(dir_buf[upDirNum].d_ino);
						for (int k = 0; k < NADDR; k++)
						{
							if (pinode->sub_dir_no[k]==0)
							{
								pinode->sub_dir_no[k] = newi;
								subDirNotFull = 1;
								iput(dir_buf[upDirNum].d_ino);
								break;
							}
						}
						if (!subDirNotFull)
						{
							cout << "dir " << dir_buf[upDirNum].d_name << " is full!" << endl;
						}
					}

				}
				else
				{
					cout << "data disk is full！" << endl;
				}
			}
			else
			{
				cout << "disk inode space is full!" << endl;
			}
		}
		else
		{
			cout << "dir space is full "<<endl;
		}
}
void freeDir(string dirname,int upDirNum)
{
		int i;
		int existed = 0;
		for (i = 1; i < 2*BLOCKSIZ / (DIRSIZ + 4); i++)
		{
			if (strcmp(dir_buf[i].d_name, dirname.c_str()) == 0)
			{
				struct inode* temp = iget(dir_buf[i].d_ino);
				if (temp->di_updir == upDirNum)
				{
					existed = 1;
					break;
				}
			}
        }//循环查找到要删除的目录的位置
        if (existed)//如果要删除的目录存在
		{
			struct inode * pinode = iget(dir_buf[i].d_ino);
			int inodeNum = dir_buf[i].d_ino;
			pinode->i_ino = inodeNum;
            if (pinode->i_flag == 'f')//如果要删除的为文件
			{
				freeFile(dirname,upDirNum);
			}
			else//如果要删除的文件为目录
			{
				queue<int>que;
				filsys.s_isize++;
                filsys.s_ninode++;//释放i节点，i节点数量增加
				filsys.s_inode[filsys.s_ninode - 1] = inodeNum;

                //超级块写回磁盘
				fseek(fd, BLOCKSIZ, SEEK_SET);
				fwrite(&filsys, sizeof(struct filsys), 1, fd);

				for (int j = 0; j < NADDR; j++)
				{
					if (pinode->sub_dir_no[j] != 0)
					{
						que.push(pinode->sub_dir_no[j]);
					}
				}
                //清空目录名称以及i节点号
				strcpy(dir_buf[i].d_name, "");
				dir_buf[i].d_ino = 0;

				fseek(fd, DATASTART, SEEK_SET);
				fwrite(dir_buf, 2*BLOCKSIZ, 1, fd);

				deleteInode(inodeNum);

                if (upDirNum != ROOTDIR)//如果目录不为根目录
				{
					struct inode* ptemp = iget(dir_buf[upDirNum].d_ino);
                    for (int j = 0; j < NADDR; j++)//取出上级I节点信息更新其中的字母子目录信息
					{
						if (ptemp->sub_dir_no[j] == i)
						{
							ptemp->sub_dir_no[j] = 0;
						}
					}
					iput(dir_buf[upDirNum].d_ino);
				}

                while (!que.empty())//循环删除字目录
				{
					int dirnum = que.front();
                    que.pop();//取出队首的子目录号
					struct inode* pinode = iget(dir_buf[dirnum].d_ino);
					int inodeNum = dir_buf[dirnum].d_ino;
					pinode->i_ino = inodeNum;
                    if (pinode->i_flag == 'f')//是文件按照文件的方式删除
					{
						freeFile(dirnum);
					}
                    else//按照目录的方式删除
					{
						filsys.s_isize++;
						filsys.s_ninode++;
						filsys.s_inode[filsys.s_ninode - 1] = inodeNum;

						fseek(fd, BLOCKSIZ, SEEK_SET);
						fwrite(&filsys, sizeof(struct filsys), 1, fd);

						for (int j = 0; j < NADDR; j++)
						{
							if (pinode->sub_dir_no[j] != 0)
							{
								que.push(pinode->sub_dir_no[j]);
							}
						}

						strcpy(dir_buf[dirnum].d_name, "");
						dir_buf[dirnum].d_ino = 0;

						fseek(fd, DATASTART, SEEK_SET);
						fwrite(dir_buf, 2*BLOCKSIZ, 1, fd);

						deleteInode(inodeNum);
					}
				}
			}
		}

}
void freeFile(string filename,int upDirNum)
{
		int i;
		int existed = 0;
		for (i = 1; i < 2*BLOCKSIZ / (DIRSIZ + 4); i++)
		{
			if (strcmp(dir_buf[i].d_name, filename.c_str()) == 0)
			{
				struct inode * temp = iget(dir_buf[i].d_ino);
				if (temp->di_updir == upDirNum)
				{
					if (upDirNum != ROOTDIR)
					{
						struct inode* pinode = iget(dir_buf[upDirNum].d_ino);
						for (int j = 0; j < NADDR; j++)
						{
							if (pinode->sub_dir_no[j] == i)
							{
								pinode->sub_dir_no[j] = 0;
							}
						}
						iput(dir_buf[upDirNum].d_ino);
					}
					existed = 1;
					break;
				}
			}
        }//查找目录中是否有要删除的文件，如果有执行下面的删除操作
		if (existed)
		{
            struct inode * pinode = iget(dir_buf[i].d_ino);//去除磁盘i节点，在内存中分配相应的空间存储i节点信息
			int inodeNum = dir_buf[i].d_ino;
			pinode->i_ino = inodeNum;
            if (pinode->i_flag == 'f')//如果要删除的目录为文件
			{
				dir_buf[i].d_ino = 0;
				strcpy(dir_buf[i].d_name, "");
                //清空要删除的目录项目

				fseek(fd, DATASTART, SEEK_SET);
				fwrite(dir_buf,2*BLOCKSIZ, 1, fd);
                //将目录表写回磁盘

				vector<int>vec;
				for (int i = 0; i<NADDR; i++)
				{
					if (pinode->di_addr[i] != 0)
					{
						vec.push_back(pinode->di_addr[i]);
					}
                }//更新上级i目录的子目录信息，字子目录号减少1个
				filsys.s_isize++;
				filsys.s_ninode++;
				filsys.s_inode[filsys.s_ninode - 1] = inodeNum;

				freeBlock(vec);
                //释放文件所占用的i节点块

				deleteInode(inodeNum);
                //删除内存i节点
			}
			else
			{
				cout << "the type that you want to deldet file is not file!" << endl;
			}
			
		}	
}
bool existFile(string filename,int upDirNum)
{
	for (int i = 1; i <2*BLOCKSIZ / (DIRSIZ + 4);i++)
	{
		if (strcmp(dir_buf[i].d_name,filename.c_str())==0)
		{
			struct inode* pinode = iget(dir_buf[i].d_ino);
			if(pinode->di_updir==upDirNum)
				return true;
		}
	}

	return false;
}
void write(struct inode* pinode,string content)
{
	vector<int>vec;
	for (int i = 0; i < NADDR; i++)
	{
		if (pinode->di_addr[i] != 0)
		{
			vec.push_back(pinode->di_addr[i]);
		}
	}
	int k = 0;
	while (!content.empty())
	{
        string temp = content.substr(0,BLOCKSIZ-1);
        qDebug()<<QString::fromStdString(temp)<<endl;
		if (temp.length() == content.length())
			content = "";
		else
		{
            content = content.substr(BLOCKSIZ-1, content.length());
		}
		if (k < vec.size())
		{
            char *chp=(char*)malloc(sizeof(char)*BLOCKSIZ);
            memset(chp,0,sizeof(char)*BLOCKSIZ);
            temp+='\0';
            strcpy(chp,temp.c_str());
			fseek(fd,DATASTART+vec[k]*BLOCKSIZ,SEEK_SET);
            fwrite(chp,BLOCKSIZ,1,fd);
            free(chp);
		}
		else
		{
			cout << "file memory is not enough!" << endl;
				break;
		}
		k++;
	}
    while(k<vec.size())
    {
        char *chp=(char*)malloc(sizeof(char)*BLOCKSIZ);
        memset(chp,0,sizeof(char)*BLOCKSIZ);
        fseek(fd,DATASTART+vec[k]*BLOCKSIZ,SEEK_SET);
        fwrite(chp,BLOCKSIZ,1,fd);
        free(chp);
        k++;
    }
}
string read(struct inode* pinode)
{
	string str = "";
	vector<int>vec;
	for (int i = 0; i < NADDR; i++)
	{
		if (pinode->di_addr[i] != 0)
		{
			vec.push_back(pinode->di_addr[i]);
		}
	}
	for (int i = 0; i < vec.size(); i++)
	{
		char ch[BLOCKSIZ];
		fseek(fd,DATASTART+vec[i]*BLOCKSIZ,SEEK_SET);
		fread(ch,BLOCKSIZ,1,fd);
		string temp(ch);
		str += temp;
	}
	return str;
}
int dirNum(string dirname,int upDirNum)
{
    for (int i = 1; i <2*BLOCKSIZ / (DIRSIZ + 4);i++)
    {
        if (strcmp(dir_buf[i].d_name, dirname.c_str()) == 0)
        {
            struct inode*temp = iget(dir_buf[i].d_ino);
            if (temp->di_updir == upDirNum)
            {
                return i;
            }
        }
    }
    return -1;
}
int  openDir(string dirname,int upDirNum)
{
		for (int i = 1; i <2*BLOCKSIZ / (DIRSIZ + 4);i++)
		{
			if (strcmp(dir_buf[i].d_name, dirname.c_str()) == 0)
			{
				struct inode*temp = iget(dir_buf[i].d_ino);
				if (temp->di_updir == upDirNum)
				{
					int inodeNum = dir_buf[i].d_ino;
					struct inode* pinode = iget(inodeNum);
					vector<int>vec;
					for (int j = 0; j < NADDR; j++)
					{
						if (pinode->sub_dir_no[j] != 0)
						{
							vec.push_back(pinode->sub_dir_no[j]);
						}
					}
					if (pinode->i_flag == 'd')
					{
						showSubDir(vec, dir_buf);
						return i;
					}
					else
					{
						cout << dirname << " is not a dir" << endl;
						return upDirNum;
					}
				}
			}
		}
		return upDirNum;
	
}
void showSubDir(vector<int>vec,struct direct *sub_dir_buf)
{
	cout << "current directory:" << endl;
	for (int i = 0; i < vec.size(); i++)
	{
		cout << sub_dir_buf[vec[i]].d_name << " " << sub_dir_buf[vec[i]].d_ino << endl;
	}
}
void write(string filename,int upDirNum,string content)
{
		for (int i = 1; i <BLOCKSIZ / (DIRSIZ + 4);i++)
		{
			if (dir_buf[i].d_name==filename)
			{
				struct inode*pinode = iget(dir_buf[i].d_ino);
				if (pinode->di_updir == upDirNum)
				{
					write(pinode, content);
					break;
				}
			}
		}
}
string read(string filename,int upDirNum)
{
		for (int i = 1; i <2*BLOCKSIZ / (DIRSIZ + 4); i++)
		{
			if (dir_buf[i].d_name == filename)
			{
				struct inode*pinode = iget(dir_buf[i].d_ino);
				if(pinode->di_updir==upDirNum)
				return read(pinode);
			}
		}
}
int openUpDir(string filename,int upDirNum)
{
	for (int i = 1; i <2 * BLOCKSIZ / (DIRSIZ + 4); i++)
	{
		if (dir_buf[i].d_name == filename)
		{
			struct inode*pinode = iget(dir_buf[i].d_ino);
			if (pinode->di_updir == upDirNum)
			{
				if (upDirNum != ROOTDIR)
				{
					struct inode* temp = iget(dir_buf[upDirNum].d_ino);
					return temp->di_updir;
				}
			}
		}
	}
	return upDirNum;
}
void freeFile(int dirnum)//根据当前目录号来删除文件
{
	struct inode * pinode = iget(dir_buf[dirnum].d_ino);
	int inodeNum = dir_buf[dirnum].d_ino;
	pinode->i_ino = inodeNum;
	if (pinode->i_flag == 'f')
	{
		dir_buf[dirnum].d_ino = 0;
		strcpy(dir_buf[dirnum].d_name, "");

		fseek(fd, DATASTART, SEEK_SET);
		fwrite(dir_buf, 2 * BLOCKSIZ, 1, fd);

		vector<int>vec;
		for (int i = 0; i<NADDR; i++)
		{
			if (pinode->di_addr[i] != 0)
			{
				vec.push_back(pinode->di_addr[i]);
			}
		}
		filsys.s_isize++;
		filsys.s_ninode++;
		filsys.s_inode[filsys.s_ninode - 1] = inodeNum;

		freeBlock(vec);

		deleteInode(inodeNum);
	}
	else
	{
		cout << "the type that you want to deldet file is not file!" << endl;
	}
}
string curPath(int dirNum)
{
    struct inode* pinode=NULL;
    vector<string>vec;
    while(dirNum!=ROOTDIR)
    {
        string str(dir_buf[dirNum].d_name);
        vec.push_back(str);
        pinode=iget(dir_buf[dirNum].d_ino);
        dirNum=pinode->di_updir;
    }
    string path="\\";
    for(int i=vec.size()-1;i>-1;i--)
    {
        path+=vec[i];
        path+="\\";
    }
    return path;
}
void readUserInfo()
{
    memset(pwd, 0, PWDNUM*sizeof(struct pwd));
    fseek(fd,DATASTART+BLOCKSIZ*2, SEEK_SET);
    fread(&pwd, sizeof(struct pwd)*PWDNUM, 1, fd);
}
void writeUserInfo()
{
    fseek(fd,DATASTART+BLOCKSIZ*2, SEEK_SET);
    fwrite(&pwd, sizeof(struct pwd)*PWDNUM, 1, fd);

    //****************************8
    for(int i=0;i<PWDNUM;i++)
    {
        if(pwd[i].p_uid!=0)
            cout<<pwd[i].p_uid<<"\t"<<pwd[i].password<<endl;
    }
}
