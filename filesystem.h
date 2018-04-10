#ifndef FILESYSTEM
#define FILESYSTEM
#include<iostream>
#include<vector>
#include<string.h>
#include<map>
using namespace std;


#include <stdio.h>
#define SUBSIZE     10
#define BLOCKSIZ    512
#define SYSOPENFILE 40
#define DIRNUM      128
#define DIRSIZ      16
#define PWDSIZ      12
#define PWDNUM      32
#define NOFILE      20
#define NADDR       10
#define NHINO       128
#define USERNUM     10
#define DINODESIZ   80
#define DINODEBLK   32
#define FILEBLK     512
#define NICFREE     50
#define NICINOD     50
#define DINODESTART 2*BLOCKSIZ
#define DATASTART   (2+DINODEBLK)*BLOCKSIZ
#define DIEMPTY     00000
#define DIFILE      01000
#define DIDIR       02000
#define UDIREAD     00001
#define UDIWRITE    00002
#define UDIEXICUTE  00004
#define GDIREAD     00010
#define GDIWRITE    00020
#define GDIEXICUTE  00040
#define ODIREAD     00100
#define ODIWRITE    00200
#define ODIEXICUTE  00400
#define READ        1
#define WRITE       2
#define EXICUTE     3
#define DEFAULTMODE 00777
#define IUPDATE     00002
#define SUPDATE     00001
#define FREAD       00001
#define FWRITE      00002
#define FAPPEND     00004
#define DISKFULL    65535
#define SEEK_SET    0
#define ROOTDIR     10000
#define ACCESSREAD  1
#define ACCESSWRITE 2
#define ACCESSSREADANDWRITE 3
struct OneFile{
    string filename;
    int beforeDirNum;
};
struct inode {
	struct inode *i_forw;
	struct inode *i_back;
	unsigned int i_ino;//磁盘i节点标号
	unsigned int i_count;//文件引用计数
	unsigned short di_number;
	unsigned short di_mode;
	char i_flag;
	unsigned short di_uid;
	unsigned short di_gid;
	unsigned short di_size;
	unsigned short sub_dir_no[SUBSIZE];
	unsigned short di_updir;
	unsigned int di_addr[NADDR];
};

struct dinode {
	unsigned short di_number;
	unsigned short di_mode;
	char i_flag;
	unsigned short di_uid;
	unsigned short di_gid;
	unsigned long di_size;
	unsigned short sub_dir_no[SUBSIZE];
    unsigned short di_updir;//主目录的上一级为ROOTDIR,主目录为0
	unsigned int di_addr[NADDR];
};

struct direct {
	char d_name[DIRSIZ];
	unsigned int d_ino;
};

struct filsys {
	unsigned short s_isize;//节点块数
	unsigned long s_fsize;//数据块块数
	unsigned int s_nfree;//空闲块数
	unsigned int s_free[NICFREE];
	unsigned int s_ninode;//空闲节点块数
	unsigned int s_inode[NICINOD];
	char s_fmod;
};

struct pwd {
	unsigned short p_uid;
	unsigned short p_gid;
	char password[PWDSIZ];
};

struct dir {
	struct direct direct[DIRNUM];
	int size;
};

struct hinode {
	struct inode *i_forw;
};

struct file {
	char f_flag;
	unsigned int f_count;
	struct inode *f_inode;
	unsigned long f_off;
};

struct user {
    unsigned short u_default_mode=ACCESSSREADANDWRITE;
    unsigned short u_uid=0;
    unsigned short u_gid=0;
	unsigned short u_ofile[NOFILE];
};
typedef struct rtTimes{
  int rCount=0;
  int wCount=0;
}rwTimes;
extern struct hinode hinode[NHINO];
extern FILE * fd;
extern struct direct dir_buf[2*BLOCKSIZ / (DIRSIZ + 4)];
extern struct filsys filsys;
extern struct pwd pwd[PWDNUM];
extern int cur_dir_num;
extern int before_dir_num;
extern struct user curUser;
void format();
struct inode* iget(int dinode);//获取空闲i节点的块号
void iput(int dinode);
void initHInode();
void install();
void control();
void showPassword();
vector<int> balloc(int blockNum);
void showSubDir(vector<int>vec, struct direct *sub_dir_buf);
void showSubDir(vector<int>vec, struct direct *sub_dir_buf);
string read(struct inode* pinode);
void write(struct inode* pinode, string content);
void mkdir(string dirname, int upDirNum);
void mkfile(string filename, int blockNum, int upDirNum);//分配blockNum块的名字为filename的文件
void freeDir(string dirname, int upDirNum);
void freeFile(string filename, int upDirNum);
bool existFile(string filename, int upDirNum);
int  openDir(string dirname, int upDirNum);
void showSubDir(vector<int>vec, struct direct *sub_dir_buf);
void write(string filename, int upDirNum, string content);
string read(string filename, int upDirNum);
int openUpDir(string filename, int upDirNum);
void showDir(int upDirNum);
void deleteInode(int dinode);
void freeBlock(vector<int>blockSet);
int fileSystemMain();
void freeFile(int dirnum);//根据当前目录号来删除文件
void inodeNum();
void outputFilSys();
int dirNum(string dirname,int upDirNum);
string curPath(int dirNum);
void writeUserInfo();
void writeBack();
void readUserInfo();
bool access(string filename,int upDirNum);
int emptyUserLoc();
#endif // !FILESYSTEM
