#include"filesystem.h"
#include<stdio.h>
#include <conio.h>
#include<stdlib.h>
#include<string>
#include<iostream>
using namespace std;
void control()
{
	string choice;
	while (cin >> choice)
	{
		if (choice == "create")
		{
			string filename;
			cout << "filename:";
			cin >> filename;
			if (!existFile(filename, cur_dir_num))
				mkfile(filename, 10, cur_dir_num);//��Ŀ¼�´����ļ���
			else
			{
				cout << filename << " is exieted!" << endl;
			}
		}
		if (choice == "newdir")
		{
			string dirname;
			cout << "dirname:";
			cin >> dirname;
			if (!existFile(dirname, cur_dir_num))
				mkdir(dirname, cur_dir_num);//��Ŀ¼�´����ļ���
			else
			{
				cout << dirname << " is exieted!" << endl;
			}
		}
		else if (choice == "dir")
		{
			showDir(cur_dir_num);
		}
		else if (choice == "delete")
		{
			string filename;
			cout << "filename:";
			cin >> filename;
			if (existFile(filename, cur_dir_num))//cur_inode_pointerĿ¼���Ƿ�����ļ�filename;
			{
				freeDir(filename, cur_dir_num);
				showDir(cur_dir_num);
			}
			else
			{
				cout << "dir dirTest is not existed!" << endl;

			}
		}
		else if (choice == "open")
		{
			string dirname;
			cout << "dirname:";
			cin >> dirname;
			cur_dir_num = openDir(dirname, cur_dir_num);
		}
		else if (choice == "write")
		{
			string filename;
			cout << "filename:";
			cin >> filename;
			if (existFile(filename, cur_dir_num))
			{
				cout << "content:";
				const int len = 4000;
				char ch[len];
				getchar();
				cin.getline(ch,len);
				string content(ch);
				write(filename, cur_dir_num, content);
			}
			else
			{
				cout << filename << " is not existed!" << endl;
			}

		}
		else if (choice == "read")
		{
			string filename;
			cout << "filename:";
			cin >> filename;
			string content;
			if (existFile(filename, cur_dir_num))
			{
				content = read(filename, cur_dir_num);
				cout << endl << content << endl;
			}
			else
			{
				cout << filename << " is not existed!" << endl;
			}
		}
		else if (choice == "..")
		{
			string filename;
			cout << "filename:" << endl;
			cin >> filename;
			cur_dir_num = openUpDir(filename, cur_dir_num);
			showDir(cur_dir_num);
		}
		else if (choice == "main")
		{
			cur_dir_num = ROOTDIR;
			showDir(cur_dir_num);
		}
		else if (choice=="inodenum")
		{
			inodeNum();
		}
		//outputFilSys();
	}
	//cout << "create dir dirTest!" << endl;
	//if (_getch() == 'c')
	//{
	//	if(!existFile("dirTest",NULL))
	//	mkdir("dirTest", NULL);//��Ŀ¼�´����ļ���
	//	else
	//	{
	//		cout << "file " << "dirTest is exieted!" << endl;
	//	}
	//}
	//cout << "create file file.txt" << endl;
	//if (_getch() == 'c')
	//{
	//	if (!existFile("file.txt", NULL))
	//		mkfile("file.txt",10,NULL);//��Ŀ¼�´����ļ���
	//	else
	//	{
	//		cout << "file " << "file.txt is exieted!" << endl;
	//	}
	//}
	//if (_getch() == 'c')
	//{
	//	if (!existFile("file1.txt", NULL))
	//		mkfile("file1.txt", 10, NULL);//��Ŀ¼�´����ļ���
	//	else
	//	{
	//		cout << "file " << "file1.txt is exieted!" << endl;
	//	}
	//}
	//char ch;
	//switch (ch=_getch())
	//{
	//case 'c': {
	//		if (!existFile("dirTest", NULL))
	//			mkdir("dirTest",NULL);//��Ŀ¼�´����ļ���
	//		else
	//		{
	//			cout << "dir " << "dirTest is exieted!" << endl;
	//		}

	//}break;
	//case 'd': {
	//	cout << "delete dir dirTest !" << endl;
	//	if (existFile("dirTest", NULL))
	//	{
	//		freeDir("dirTest", NULL);
	//		showDir(dir_buf, BLOCKSIZ / (DIRSIZ + 4));
	//	}
	//	else
	//	{
	//		cout << "dir dirTest is not existed!" << endl;

	//	}
	//
	//}break;

	//default:
	//	break;
	//}
	//if (_getch() == 'd')
	//{
	//
	//}
	//if (_getch() == 'd')
	//{
	//	cout << "delete dir dirTest!" << endl;
	//	if (existFile("dirTest", NULL))
	//	{
	//		freeDir("dirTest", NULL);
	//		showDir(dir_buf, BLOCKSIZ / (DIRSIZ + 4));
	//	}
	//	else
	//	{
	//		cout << "dir dirTest is not existed!" << endl;

	//	}
	//}

}
void inodeNum()
{
    cout<<"***********************************"<<endl;
    int num=0;
	for (int i = 0; i <NHINO;i++)
	{
		int count = 0;
		struct inode*pinode = hinode[i].i_forw->i_forw;
		while (pinode != NULL)
		{
			count++;
			pinode = pinode->i_forw;
		}
		if (count)
		{
			cout << i << "-----------------------------:" << count << endl;
            num++;
		}
	}
    cout<<"num:"<<num<<endl;
}
void outputFilSys()
{
	cout << "-----------------------------------------" << endl;
	cout << filsys.s_fmod << endl;
	cout << "�������ݿ�:" << endl;
	for (int i = 0; i < 50; i++)
	{
		cout << filsys.s_free[i] << "\t";
	}
	cout << endl << "�������ݿ�����" << filsys.s_nfree << endl;;
	cout << "���нڵ�飺" << endl;
	for (int i = 0; i < 50; i++)
	{
		cout << filsys.s_inode[i]<< "\t";
	}
	cout << endl;
	cout << "����i�ڵ������" << filsys.s_ninode << endl;
	cout << "�ܵĿ������ݿ�����" << filsys.s_fsize << endl;
	cout << "�ܵĿ��нڵ����:" << filsys.s_isize << endl;
}
