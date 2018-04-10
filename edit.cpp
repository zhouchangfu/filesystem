#include "edit.h"
#include "ui_edit.h"
#include<QDebug>

Edit::Edit(string filename,int upDirNum,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Edit)
{
    ui->setupUi(this);
    this->filename=filename;
    this->upDirNum=upDirNum;
    string content=read(filename,upDirNum);
    ui->textEdit->setText(QString::fromStdString(content));
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    string path=curPath(cur_dir_num)+filename;
    MainWindow::rTimesPlus(path);
}

Edit::~Edit()
{
    delete ui;
}

void Edit::on_pushButton_clicked()
{
    QString qstr=ui->textEdit->toPlainText();
    qDebug()<<qstr<<endl;
    string content=qstr.toStdString();
    write(filename,upDirNum,content);
    string path=curPath(cur_dir_num)+filename;
    MainWindow::wTimesPlus(path);
}
