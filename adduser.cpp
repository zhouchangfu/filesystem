#include"filesystem.h"
#include<stdio.h>
#include<QWidget>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<QLineEdit>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QPushButton>
using namespace std;
int emptyUserLoc()
{
    readUserInfo();
    int existedEmpty=0;
    int index=-1;
    for(int i=0;i<PWDNUM;i++)
    {
        if(pwd[i].p_uid==0)
            {
            existedEmpty=true;
            index=i;
            break;
        }
    }
    return index;
}
