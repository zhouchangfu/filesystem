#include "mainwindow.h"
#include"filesystem.h"
#include <QApplication>
#include<QTextCodec>
#include<QTreeView>
#include<QFileSystemModel>
#include<QListWidget>
#include <conio.h>
#include"filesystem.h"
#include<windows.h>
#include<iostream>
using namespace std;
struct filsys filsys;
struct hinode hinode[NHINO];
struct direct dir_buf[2*BLOCKSIZ / (DIRSIZ + 4)];
struct pwd pwd[PWDNUM];
int cur_dir_num=ROOTDIR;
int before_dir_num=ROOTDIR;
FILE *fd;
struct user curUser;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    MainWindow w;
    w.show();
   //fileSystemMain();
    return a.exec();
}
