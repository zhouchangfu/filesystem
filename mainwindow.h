#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QApplication>
#include<QTextCodec>
#include<QTreeView>
#include<QFileSystemModel>
#include<QListWidget>
#include <QMainWindow>
#include<QWheelEvent>
#include"filesystem.h"
#include<systable.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void contextMenuEvent(QContextMenuEvent *event);
public slots:
    void newFile();
    void newDir();
    void deleteDir();
    void editFile();
    void dealItemClicked(QListWidgetItem*item);
    void showDirInGui(int upDirNum);
private slots:
    void on_actionFormat_triggered();

    void on_actionClose_triggered();

    void on_actionUp_triggered();
    void enLarge();
    void enSmall();
    void copy();
    void paste();
    void rename();
    void showProperty();

    void on_actionLarge_triggered();

    void on_actionSmall_triggered();
    void wheelEvent(QWheelEvent * event);

    void on_actionHome_triggered();

    void on_actionOpen_new_window_triggered();

    void on_actionShowTable_triggered();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    bool login(int uid,string password);

    void on_actionLogin_triggered();

    void on_actionAddUser_triggered();
    void comfirmAdd();
    void cancelAdd();

    void on_actionUsersInfo_triggered();

private:
    Ui::MainWindow *ui;
    int height=30;
    int width=30;
    struct OneFile onefile;
    QLineEdit *userEdit;
    QLineEdit *userpwd1;
    QLineEdit *userpwd2;
    QWidget * widget;

public:
    static map<string,rwTimes>dirRwTimes;
    static void wTimesPlus(string path);
    static void rTimesPlus(string path);
    void addUser();

};

#endif // MAINWINDOW_H
