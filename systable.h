#ifndef SYSTABLE_H
#define SYSTABLE_H
#include"filesystem.h"
#include <QDialog>
#include<QTableWidget>
#include<QTableWidgetItem>
#include<string>
#include<map>

namespace Ui {
class SysTable;
}

class SysTable : public QDialog
{
    Q_OBJECT

public:
    explicit SysTable(map<string,rwTimes>mymap,QDialog *parent = 0);
    ~SysTable();

private:
    Ui::SysTable *ui;
    map<string,rwTimes>mymap;
};

#endif // SYSTABLE_H
