#include"filesystem.h"
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include<vector>
#include<queue>
#include<qDebug>
using namespace std;
void mkdir(string dirname,int upDirNum)//����Ŀ¼
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
        //���ҿ��е�Ŀ¼��
		if (existed)//���ڿռ���Դ���Ŀ¼
		{
            if (filsys.s_ninode > 0)//������ڿ��нڵ��
			{
                //*****************************test*******


				int inodeNum = filsys.s_inode[filsys.s_ninode-1];
				filsys.s_inode[filsys.s_ninode - 1] = 0;
				filsys.s_ninode--;
                filsys.s_isize--;//���нڵ��������
				struct inode* pinode = iget(inodeNum);
				for (int m = 0; m < NADDR; m++)
				{
					pinode->di_addr[m] = 0;
					pinode->sub_dir_no[m] = 0;
                }//���нڵ����Ŀ¼��ռ�ÿ�������
                pinode->i_ino = inodeNum;//��¼i�ڵ���̿��
                pinode->i_flag = 'd';//��ʶһ���ļ�������ΪĿ¼
                pinode->di_size = 0;//Ŀ¼�Ĵ�СΪ0
                pinode->di_number = 0;//��Ŀ¼��ĿΪ0
                pinode->di_updir = upDirNum;//�ϼ�Ŀ¼����
                pinode->di_mode=ACCESSSREADANDWRITE;//���ö�д����Ȩ��
                pinode->di_uid=curUser.u_uid;//����I�ڵ�id��
                strcpy(dir_buf[i].d_name,dirname.c_str());//����Ŀ¼����
                dir_buf[i].d_ino = inodeNum;//����i�ڵ���
                iput(inodeNum);//��i�ڵ�д�ش���

				//д��mainĿ¼��
				fseek(fd,DATASTART,SEEK_SET);
				fwrite(dir_buf,2*BLOCKSIZ,1,fd);
				showDir(upDirNum);

				//д�ᳬ����
				fseek(fd,BLOCKSIZ,SEEK_SET);
				fwrite(&filsys,sizeof(struct filsys),1,fd);

				int subDirNotFull = 0;
                if (upDirNum != ROOTDIR)//�жϵ�ǰĿ¼��Ϊ��Ŀ¼�Ļ�
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
                    }//���ϼ�Ŀ¼����Ŀ¼����һ��Ԫ��
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
void mkfile(string filename,int blockNum,int upDirNum)//����blockNum�������Ϊfilename���ļ�
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
		if (existed)//���������ڵ�ռ���Դ���Ŀ¼
		{
			if (filsys.s_ninode > 0)
			{
				if (filsys.s_fsize >= blockNum)//�������ݿ���Է����������ļ�
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
					//д��i�ڵ�
					iput(inodeNum);
					//д����Ŀ¼��
					fseek(fd, DATASTART, SEEK_SET);
					fwrite(dir_buf,2*BLOCKSIZ, 1, fd);
					showDir(upDirNum);

					//д�ᳬ����
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
					cout << "data disk is full��" << endl;
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
        }//ѭ�����ҵ�Ҫɾ����Ŀ¼��λ��
        if (existed)//���Ҫɾ����Ŀ¼����
		{
			struct inode * pinode = iget(dir_buf[i].d_ino);
			int inodeNum = dir_buf[i].d_ino;
			pinode->i_ino = inodeNum;
            if (pinode->i_flag == 'f')//���Ҫɾ����Ϊ�ļ�
			{
				freeFile(dirname,upDirNum);
			}
			else//���Ҫɾ�����ļ�ΪĿ¼
			{
				queue<int>que;
				filsys.s_isize++;
                filsys.s_ninode++;//�ͷ�i�ڵ㣬i�ڵ���������
				filsys.s_inode[filsys.s_ninode - 1] = inodeNum;

                //������д�ش���
				fseek(fd, BLOCKSIZ, SEEK_SET);
				fwrite(&filsys, sizeof(struct filsys), 1, fd);

				for (int j = 0; j < NADDR; j++)
				{
					if (pinode->sub_dir_no[j] != 0)
					{
						que.push(pinode->sub_dir_no[j]);
					}
				}
                //���Ŀ¼�����Լ�i�ڵ��
				strcpy(dir_buf[i].d_name, "");
				dir_buf[i].d_ino = 0;

				fseek(fd, DATASTART, SEEK_SET);
				fwrite(dir_buf, 2*BLOCKSIZ, 1, fd);

				deleteInode(inodeNum);

                if (upDirNum != ROOTDIR)//���Ŀ¼��Ϊ��Ŀ¼
				{
					struct inode* ptemp = iget(dir_buf[upDirNum].d_ino);
                    for (int j = 0; j < NADDR; j++)//ȡ���ϼ�I�ڵ���Ϣ�������е���ĸ��Ŀ¼��Ϣ
					{
						if (ptemp->sub_dir_no[j] == i)
						{
							ptemp->sub_dir_no[j] = 0;
						}
					}
					iput(dir_buf[upDirNum].d_ino);
				}

                while (!que.empty())//ѭ��ɾ����Ŀ¼
				{
					int dirnum = que.front();
                    que.pop();//ȡ�����׵���Ŀ¼��
					struct inode* pinode = iget(dir_buf[dirnum].d_ino);
					int inodeNum = dir_buf[dirnum].d_ino;
					pinode->i_ino = inodeNum;
                    if (pinode->i_flag == 'f')//���ļ������ļ��ķ�ʽɾ��
					{
						freeFile(dirnum);
					}
                    else//����Ŀ¼�ķ�ʽɾ��
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
        }//����Ŀ¼���Ƿ���Ҫɾ�����ļ��������ִ�������ɾ������
		if (existed)
		{
            struct inode * pinode = iget(dir_buf[i].d_ino);//ȥ������i�ڵ㣬���ڴ��з�����Ӧ�Ŀռ�洢i�ڵ���Ϣ
			int inodeNum = dir_buf[i].d_ino;
			pinode->i_ino = inodeNum;
            if (pinode->i_flag == 'f')//���Ҫɾ����Ŀ¼Ϊ�ļ�
			{
				dir_buf[i].d_ino = 0;
				strcpy(dir_buf[i].d_name, "");
                //���Ҫɾ����Ŀ¼��Ŀ

				fseek(fd, DATASTART, SEEK_SET);
				fwrite(dir_buf,2*BLOCKSIZ, 1, fd);
                //��Ŀ¼��д�ش���

				vector<int>vec;
				for (int i = 0; i<NADDR; i++)
				{
					if (pinode->di_addr[i] != 0)
					{
						vec.push_back(pinode->di_addr[i]);
					}
                }//�����ϼ�iĿ¼����Ŀ¼��Ϣ������Ŀ¼�ż���1��
				filsys.s_isize++;
				filsys.s_ninode++;
				filsys.s_inode[filsys.s_ninode - 1] = inodeNum;

				freeBlock(vec);
                //�ͷ��ļ���ռ�õ�i�ڵ��

				deleteInode(inodeNum);
                //ɾ���ڴ�i�ڵ�
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
void freeFile(int dirnum)//���ݵ�ǰĿ¼����ɾ���ļ�
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
