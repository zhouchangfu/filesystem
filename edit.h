#ifndef EDIT_H
#define EDIT_H
#include<string>
#include<iostream>
#include"filesystem.h"
using namespace std;
#include <QDialog>
#include"mainwindow.h"

namespace Ui {
class Edit;
}

class Edit : public QDialog
{
    Q_OBJECT

public:
    explicit Edit(string filename,int upDirNum,QWidget *parent = 0);
    ~Edit();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Edit *ui;
    string filename;
    int upDirNum;
};

#endif // EDIT_H
