#include "systable.h"
#include "ui_systable.h"
#include<sstream>
#include<QString>
#include<QStringList>

SysTable::SysTable(map<string,rwTimes>mymap,QDialog *parent) :
    QDialog(parent),mymap(mymap),
    ui(new Ui::SysTable)
{
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    ui->setupUi(this);
    int rows=mymap.size();
    ui->tableWidget->setRowCount(rows);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("filename"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("read count"));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("write count"));
    map<string,rwTimes>::iterator iter=mymap.begin();
    map<string,rwTimes>::iterator iterEnd=mymap.end();
    int row=0;
    for(;iter!=iterEnd;iter++)
    {
        QString pathname=QString::fromStdString(iter->first);
        QTableWidgetItem *path = new QTableWidgetItem(pathname);
        int num=iter->second.rCount;
        int num1=iter->second.wCount;
        ostringstream os;
        os<<num;
        string numstr=os.str();
        os.clear();
        ostringstream os1;
        os1<<num1;
        string num1str=os1.str();
        QString numqstr=QString::fromStdString(numstr);
        QString num1qstr=QString::fromStdString(num1str);
        QTableWidgetItem *count1 = new QTableWidgetItem(numqstr);
        QTableWidgetItem *count2 = new QTableWidgetItem(num1qstr);
        ui->tableWidget->setItem(row,0,path);
        ui->tableWidget->setItem(row,1,count1);
        ui->tableWidget->setItem(row,2,count2);
        row++;
    }
    ui->tableWidget->show();
}

SysTable::~SysTable()
{
    delete ui;
}
