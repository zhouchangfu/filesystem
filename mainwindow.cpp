#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"QMessageBox"
#include"edit.h"
#include<QInputDialog>
#include<QProcess>
#include<sstream>
#include<QPushButton>


map<string,rwTimes> MainWindow::dirRwTimes=map<string,rwTimes>();
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    onefile.filename="";
    onefile.beforeDirNum=0;
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(dealItemClicked(QListWidgetItem*)));
    initHInode();
    install();//安装磁盘系统
    ui->actionUp->setIcon(QIcon("://new//prefix1//up.jpg"));
    ui->actionLarge->setIcon(QIcon("://new//prefix1//large.jpg"));
    ui->actionSmall->setIcon(QIcon("://new//prefix1//small.jpg"));
    ui->actionHome->setIcon(QIcon("://new//prefix1//home.jpg"));
    showDirInGui(cur_dir_num);
    showDir(ROOTDIR);
    setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{

    writeBack();
    fclose(fd);
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu=new QMenu(this);
    QAction *p0=new QAction(QIcon("://new//prefix1//file.jpg"),"newfile",this);
    QAction *p1=new QAction(QIcon("://new//prefix1//dir.jpg"),"newdir",this);
    QAction *p2=new QAction(QIcon(),"delete",this);
    QAction *p3=new QAction(QIcon(),"edit",this);
    QAction *p4=new QAction(QIcon(),"copy",this);
    QAction *p5=new QAction(QIcon(),"paste",this);
    QAction *p6=new QAction(QIcon(),"rename",this);
    QAction *p7=new QAction(QIcon(),"property",this);
    connect(p0,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(p1,SIGNAL(triggered()),this,SLOT(newDir()));
    connect(p2,SIGNAL(triggered()),this,SLOT(deleteDir()));
    connect(p3,SIGNAL(triggered()),this,SLOT(editFile()));
    connect(p4,SIGNAL(triggered()),this,SLOT(copy()));
    connect(p5,SIGNAL(triggered()),this,SLOT(paste()));
    connect(p6,SIGNAL(triggered()),this,SLOT(rename()));
    connect(p7,SIGNAL(triggered()),this,SLOT(showProperty()));
    menu->addAction(p0);
    menu->addAction(p1);
    menu->addAction(p2);
    menu->addAction(p3);
    menu->addAction(p4);
    menu->addAction(p5);
    menu->addAction(p6);
    menu->addAction(p7);
    menu->move(cursor().pos());
    menu->setMinimumSize(height,width);
    menu->show();
}

void MainWindow::newFile()
{
  bool isOk=false;
  QString text = QInputDialog::getText(NULL, "Password",
                                         "Please input file name",
                                         //  QLineEdit::Password,    //输入的是密码，不显示明文
                                         QLineEdit::Normal,          //输入框明文
                                         NULL,
                                         &isOk);
  string filename=text.toStdString();
  if(isOk)
  {
      if(existFile(filename,cur_dir_num))
      {
          QString str="filename "+text+" is existed!";
          QMessageBox::question(this,"tip",str,0);
      }
      else
      {
          mkfile(filename,10,cur_dir_num);
          showDirInGui(cur_dir_num);
      }
  }
}

void MainWindow::newDir()
{
    bool isOk=false;
    QString text = QInputDialog::getText(NULL, "Password",
                                           "Please input directory name",
                                           //  QLineEdit::Password,    //输入的是密码，不显示明文
                                           QLineEdit::Normal,          //输入框明文
                                           NULL,
                                           &isOk);
    string filename=text.toStdString();
    if(isOk)
    {
        if(existFile(filename,cur_dir_num))
        {
            QString str="filename "+text+" is existed!";
            QMessageBox::question(this,"tip",str,0);
        }
        else
        {
            mkdir(filename,cur_dir_num);
           /* int i=dirNum(filename,cur_dir_num);
            struct inode* pinode=iget(i);
            cout<<"pinode->i_flag:"<<pinode->i_flag<<" after mkdir!"<<endl;
            cout<<"i=="<<i<<endl;*/
            showDirInGui(cur_dir_num);
        }
    }
}

void MainWindow::deleteDir()
{
    if(ui->listWidget->count()==0)
        return ;
    QString filename=ui->listWidget->currentItem()->text();
    if(!existFile(filename.toStdString(),cur_dir_num))
    {
        QMessageBox::question(this,"tip",filename+"is not existed!",0);
    }
    else
    {
        if(access(filename.toStdString(),cur_dir_num))
        freeDir(filename.toStdString(),cur_dir_num);
        else
        {
            QMessageBox::question(this,"tip","you has not access to delete current file!",0);
        }
    }
    showDirInGui(cur_dir_num);
}

void MainWindow::editFile()
{
    if(ui->listWidget->count()==0)
        return ;
    QString dirname=ui->listWidget->currentItem()->text();
    int i=dirNum(dirname.toStdString(),cur_dir_num);
    if(i==-1)
    {
        QMessageBox::question(this,"error!","当前目录名称在目录缓存中不存在!",0);
    }
    else
    {
          struct inode*pinode=iget(dir_buf[i].d_ino);
          if(pinode->i_flag=='d')
          {
              if(access(dirname.toStdString(),cur_dir_num))
              {
              before_dir_num=cur_dir_num;
              cur_dir_num=openDir(dirname.toStdString(),cur_dir_num);
              }
              else
              {
                  QMessageBox::question(this,"tip","you has not access to open current directory!",0);
              }
          }
          else if(pinode->i_flag=='f')
          {
              if(access(dirname.toStdString(),cur_dir_num))
              {
                  Edit edit(dirname.toStdString(),cur_dir_num);
                  edit.show();
                  edit.exec();
              }
              else
              {
                  QMessageBox::question(this,"tip","you has not access to open current file!",0);
              }
          }
    }
    showDirInGui(cur_dir_num);
}

void MainWindow::dealItemClicked(QListWidgetItem *item)
{
    QString dirname=item->text();
    int i=dirNum(dirname.toStdString(),cur_dir_num);
    if(i==-1)
    {
        QMessageBox::question(this,"error!","当前目录名称在目录缓存中不存在!",0);
    }
    else
    {
        struct inode*pinode=iget(dir_buf[i].d_ino);
        if(pinode->i_flag=='d')
        {
            if(access(dirname.toStdString(),cur_dir_num))
            {
            before_dir_num=cur_dir_num;
            cur_dir_num=openDir(dirname.toStdString(),cur_dir_num);
            }
            else
            {
                QMessageBox::question(this,"tip","you has not access open current directory!",0);
            }
        }
        else if(pinode->i_flag=='f')
        {
            if(access(dirname.toStdString(),cur_dir_num))
            {
                Edit edit(dirname.toStdString(),cur_dir_num);
                edit.show();
                edit.exec();
            }
            else
            {
                QMessageBox::question(this,"tip","you has not access open current file!",0);
            }
        }
    }
    showDirInGui(cur_dir_num);
}

void MainWindow::showDirInGui(int upDirNum)
{
    ui->listWidget->clear();
    for (int i = 0; i <2 * BLOCKSIZ / (DIRSIZ + 4); i++)
    {
        if (dir_buf[i].d_ino != 0)
        {
            struct inode* pinode = iget(dir_buf[i].d_ino);
            string filename(dir_buf[i].d_name);
            if(pinode->di_updir== upDirNum)
            {
                struct inode* pinode=iget(dir_buf[i].d_ino);
                if(pinode->i_flag=='d')
                {
                     QListWidgetItem *item=new QListWidgetItem(QIcon("://new//prefix1//dir.jpg"),filename.c_str());
                     item->setSizeHint(QSize(height,width));
                     ui->listWidget->setIconSize(QSize(height,width));
                     ui->listWidget->addItem(item);
                }
                else  if(pinode->i_flag=='f')
                {
                    QListWidgetItem *item=new QListWidgetItem(QIcon("://new//prefix1//file.jpg"),filename.c_str());
                    item->setSizeHint(QSize(height,width));
                    ui->listWidget->setIconSize(QSize(height,width));
                    ui->listWidget->addItem(item);
                }
            }
        }
    }
    ui->lineEdit->setText(QString::fromStdString(curPath(cur_dir_num)));
    inodeNum();
    //connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(dealItemClicked(QListWidgetItem*)));
}

void MainWindow::on_actionFormat_triggered()
{
    if(curUser.u_uid!=0)
    {
    format();
    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    this->close();
    }
    else
    {
        QMessageBox::question(this,"tip","you has not access to fortmat the disk!",0);
    }
}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}

void MainWindow::on_actionUp_triggered()
{
    if(ui->listWidget->count()>0)
    {
    QString filename=ui->listWidget->item(0)->text();
    cur_dir_num=openUpDir(filename.toStdString(),cur_dir_num);
    }
    else
    {
    cur_dir_num=before_dir_num;
    }
    showDirInGui(cur_dir_num);
}

void MainWindow::enLarge()
{
    for(int i=0;i<ui->listWidget->count();i++)
    {
        height=ui->listWidget->item(i)->sizeHint().height()+3;
        width=ui->listWidget->item(i)->sizeHint().width()+3;
        ui->listWidget->item(i)->setSizeHint(QSize(height,width));
        ui->listWidget->setIconSize(QSize(height,width));
    }
}

void MainWindow::enSmall()
{
    for(int i=0;i<ui->listWidget->count();i++)
    {

        height=ui->listWidget->item(i)->sizeHint().height()-3;
        width=ui->listWidget->item(i)->sizeHint().width()-3;
        width=width>0?width:2;
        height=height>0?height:2;
        ui->listWidget->item(i)->setSizeHint(QSize(height,width));
        ui->listWidget->setIconSize(QSize(height,width));
    }
}

void MainWindow::copy()
{
    if(ui->listWidget->count()==0)
        return ;
    string filename=ui->listWidget->currentItem()->text().toStdString();
    if(access(filename,cur_dir_num))
    {
    onefile.filename=ui->listWidget->currentItem()->text().toStdString();
    onefile.beforeDirNum=cur_dir_num;
    }
    else
    {
        QMessageBox::question(this,"tip","you has not access open current file!",0);
    }
}

void MainWindow::paste()
{
    if(existFile(onefile.filename,cur_dir_num))
    {
        string str=onefile.filename+" is existed in current directory!";
        QMessageBox::question(this,"tip",QString::fromStdString(str),0);
    }
    else
    {
        int dirnum=dirNum(onefile.filename,onefile.beforeDirNum);
        struct inode * pinode=iget(dir_buf[dirnum].d_ino);
        vector<int>vec;
        for(int i=0;i<NADDR;i++)
        {
            if(pinode->di_addr[i]!=0)
                vec.push_back(i);
        }
        if(pinode->i_flag=='f')
        {
            int size=vec.size();
            mkfile(onefile.filename,size,cur_dir_num);
            string content=read(onefile.filename,onefile.beforeDirNum);
            write(onefile.filename,cur_dir_num,content);
        }

    }
    showDirInGui(cur_dir_num);
}

void MainWindow::rename()
{
    if(ui->listWidget->count()==0)
        return ;
    string filename=ui->listWidget->currentItem()->text().toStdString();
    if(!access(filename,cur_dir_num))
    {
        QMessageBox::question(this,"tip","you has not access to rename the file!",0);
        return;
    }

    int dirnum=dirNum(filename,cur_dir_num);
    string oldName=dir_buf[dirnum].d_name;
    bool isOk=false;
    QString text = QInputDialog::getText(NULL, "Password",
                                           "Please input directory name",
                                           //  QLineEdit::Password,    //输入的是密码，不显示明文
                                           QLineEdit::Normal,          //输入框明文
                                           NULL,
                                           &isOk);
    string newName=text.toStdString();
    if(isOk)
    {
        strcpy(dir_buf[dirnum].d_name,"");
        if(existFile(newName,cur_dir_num))
        {
            string str="file name"+newName+" has existed in current directory！";
            QMessageBox::question(this,"tip",QString::fromStdString(str),0);
            strcpy(dir_buf[dirnum].d_name,oldName.c_str());
        }
        else
        {
           strcpy(dir_buf[dirnum].d_name,newName.c_str());
        }
    }
    showDirInGui(cur_dir_num);
}

void MainWindow::showProperty()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    if(ui->listWidget->count()==0)
        return ;
    string filename=ui->listWidget->currentItem()->text().toStdString();
    int dirnum=dirNum(filename,cur_dir_num);
    struct inode* pinode=iget(dir_buf[dirnum].d_ino);
    QTableWidget *tableWidget=new QTableWidget();
    tableWidget->setRowCount(6);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("property"));
    tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("value"));
//    ostringstream os;
//    os<<pinode->di_number;
//    tableWidget->setItem(0,0,new QTableWidgetItem("relative file count"));
//    tableWidget->setItem(0,1,new QTableWidgetItem(QString::fromStdString(os.str())));


    ostringstream os1;
    os1<<pinode->di_mode;
    tableWidget->setItem(0,0,new QTableWidgetItem("file access mode"));
    tableWidget->setItem(0,1,new QTableWidgetItem(QString::fromStdString(os1.str())));


    string type="";
    type+=pinode->i_flag;
    tableWidget->setItem(1,0,new QTableWidgetItem("file type"));
    tableWidget->setItem(1,1,new QTableWidgetItem(QString::fromStdString(type)));

    ostringstream os2;
    os2<<pinode->di_uid;
    tableWidget->setItem(2,0,new QTableWidgetItem("user id"));
    tableWidget->setItem(2,1,new QTableWidgetItem(QString::fromStdString(os2.str())));

    ostringstream os3;
    os3<<pinode->di_gid;
    tableWidget->setItem(3,0,new QTableWidgetItem("user group id"));
    tableWidget->setItem(3,1,new QTableWidgetItem(QString::fromStdString(os3.str())));

    int num=0;
    for(int i=0;i<NADDR;i++)
    {
        if(pinode->sub_dir_no[i]!=0)
        {
            num++;
        }
    }
    ostringstream os4;
    os4<<num;
    tableWidget->setItem(4,0,new QTableWidgetItem("sub file count"));
    tableWidget->setItem(4,1,new QTableWidgetItem(QString::fromStdString(os4.str())));

    string chain="";
    for(int i=0;i<NADDR;i++)
    {
        if(pinode->di_addr[i]!=0)
        {
            ostringstream os;
            os<<pinode->di_addr[i];
            chain+=os.str();
            chain+=" ";
        }
    }
    tableWidget->setItem(5,0,new QTableWidgetItem("block chain"));
    tableWidget->setItem(5,1,new QTableWidgetItem(QString::fromStdString(chain)));
    tableWidget->setFrameShape(QFrame::NoFrame);
    for(int i=0;i<tableWidget->rowCount();i++)
    {
        tableWidget->item(i,0)->setFont(QFont("Helvetica")); //设置字体
        tableWidget->item(i,0)->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
        tableWidget->item(i,0)->setTextColor(QColor(200,111,30)); //设置文字颜色

        tableWidget->item(i,1)->setFont(QFont("Helvetica")); //设置字体
        tableWidget->item(i,1)->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
        tableWidget->item(i,1)->setTextColor(QColor(200,111,30)); //设置文字颜色
    }
    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    tableWidget->resizeColumnsToContents();
    tableWidget->resizeRowsToContents();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    tableWidget->show();

}
void MainWindow::on_actionLarge_triggered()
{
    enLarge();
}

void MainWindow::on_actionSmall_triggered()
{
    enSmall();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0){
           enLarge();
        }else{
           enSmall();
        }
}

void MainWindow::on_actionHome_triggered()
{
    cur_dir_num=ROOTDIR;
    showDirInGui(cur_dir_num);
}

void MainWindow::on_actionOpen_new_window_triggered()
{
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void MainWindow::wTimesPlus(string path)
{
    dirRwTimes[path].wCount++;
}

void MainWindow::rTimesPlus(string path)
{
     dirRwTimes[path].rCount++;
}

void MainWindow::on_actionShowTable_triggered()
{
    SysTable sysTable(dirRwTimes);
    sysTable.show();
    sysTable.exec();
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{


}

bool MainWindow::login(int uid, string password)
{
    if(uid!=0)
    {
        readUserInfo();
        for(int i=0;i<PWDNUM;i++)
        {
            if(pwd[i].p_uid==uid)
            {
                curUser.u_gid=pwd[i].p_gid;
                if(strcmp(password.c_str(),pwd[i].password)==0)
                    return true;
            }
        }
        return false;
    }
    else
    {
        return false;
    }

}

void MainWindow::on_actionLogin_triggered()
{
    if(curUser.u_uid!=0)
    {
        QMessageBox::question(this,"tip","you is has logined!",0);
        return ;
    }
    bool isOk=false;
    QString uidqstr = QInputDialog::getText(NULL, "Password",
                                           "Please input user id",
                                           //  QLineEdit::Password,    //输入的是密码，不显示明文
                                           QLineEdit::Normal,          //输入框明文
                                           NULL,
                                           &isOk);
    string uidstr=uidqstr.toStdString();
    if(isOk)
    {
        isOk=false;
        QString pwd = QInputDialog::getText(NULL, "Password",
                                               "Please input user password",
                                               //  QLineEdit::Password,    //输入的是密码，不显示明文
                                               QLineEdit::Normal,          //输入框明文
                                               NULL,
                                               &isOk);
        if(isOk)
        {
            string password=pwd.toStdString();
            int uid=atoi(uidstr.c_str());
            if(login(uid,password))
            {
                curUser.u_uid=uid;
                if(!existFile(uidstr,cur_dir_num))
                {
                    mkdir(uidstr,cur_dir_num);
                }
            }
            else
            {
                QMessageBox::question(this,"tip","login error!",0);
            }
        }
    }
    showDirInGui(cur_dir_num);
}

void MainWindow::on_actionAddUser_triggered()
{
    if(curUser.u_uid==0)
    {
        QMessageBox::question(this,"tip","please login first!",0);
        return;
    }
    else if(curUser.u_uid!=2116)
    {
        QMessageBox::question(this,"tip","you have no access to add the user!",0);
        return;
    }
    addUser();
}

void MainWindow::comfirmAdd()
{

    QString uname=userEdit->text();
    QString password1=userpwd1->text();
    QString password2=userpwd2->text();
    if(password1==""||uname==""||password2=="")
    {
        QMessageBox::question(this,"tip","please input complete information!",0);
    }
    else if(password1!=password2)
    {
        QMessageBox::question(this,"tip","two password is not same!",0);
    }
    else
    {
        int uid=atoi(uname.toStdString().c_str());
        if(uid==0)
        {
            QMessageBox::question(this,"tip","user name must be is number!",0);
        }
        else
        {
            for(int i=0;i<PWDNUM;i++)
                {
                if(pwd[i].p_uid==uid)
                {
                     QMessageBox::question(this,"tip","current uid is existed!",0);
                     return;
                }
            }
            int index=emptyUserLoc();
            if(index!=-1)
            {
                strcpy(pwd[index].password,password1.toStdString().c_str());
                pwd[index].p_uid=uid;
                QMessageBox::question(this,"tip","congratulations !",0);
                writeUserInfo();
            }
            else
            {
               QMessageBox::question(this,"tip","current system user numbers is full!",0);
            }

        }
    }
}

void MainWindow::cancelAdd()
{
    widget->close();
}
void MainWindow:: addUser()
{

   widget=new QWidget();
   QVBoxLayout* vlayout=new QVBoxLayout();
   userEdit=new QLineEdit();
   userEdit->setWindowTitle("username");
   userpwd1=new QLineEdit();
   userpwd1->setWindowTitle("password");
   userpwd2=new QLineEdit();
   userpwd2->setWindowTitle("password");
   userpwd1->setEchoMode(QLineEdit::Password);
   userpwd2->setEchoMode(QLineEdit::Password);
   vlayout->addWidget((QWidget*)userEdit);
   vlayout->addWidget((QWidget*)userpwd1);
   vlayout->addWidget((QWidget*)userpwd2);
   QHBoxLayout *hlayout=new QHBoxLayout(this);
   QPushButton *add=new QPushButton(this);
   QPushButton *cancel=new QPushButton(this);
   add->setText("add");
   cancel->setText("cancel");
   hlayout->addWidget((QWidget*)add);
   hlayout->addWidget((QWidget*)cancel);
   vlayout->addLayout(hlayout);
   widget->setLayout(vlayout);
   connect(add,SIGNAL(clicked()),this,SLOT(comfirmAdd()));
   connect(cancel,SIGNAL(clicked()),this,SLOT(cancelAdd()));
   widget->show();

}

void MainWindow::on_actionUsersInfo_triggered()
{
    if(curUser.u_uid!=2116)
    {
        QMessageBox::question(this,"tip","you have no access to watch the user info!",0);
        return;
    }
    readUserInfo();
    QTableWidget *tableWidget=new QTableWidget();
    tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("UUID"));
    tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("guid"));
    tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("PASSWORD"));
//    ostringstream os;
//    os<<pinode->di_number;
//    tableWidget->setItem(0,0,new QTableWidgetItem("relative file count"));
//    tableWidget->setItem(0,1,new QTableWidgetItem(QString::fromStdString(os.str())));
    int count=0;
    for(int i=0;i<PWDNUM;i++)
    {
        if(pwd[i].p_uid!=0)
        {
            count++;
        }
    }
    tableWidget->setRowCount(count);
    tableWidget->setColumnCount(3);
    int row=0;
    for(int i=0;i<PWDNUM;i++)
    {
        if(pwd[i].p_uid!=0)
        {
            ostringstream os;
            os<<pwd[i].p_uid;
            string uid=os.str();
            ostringstream os1;
            os1<<pwd[i].p_gid;
            string guid=os1.str();
            string password(pwd[i].password);
            tableWidget->setItem(row,0,new QTableWidgetItem(QString::fromStdString(uid)));
            tableWidget->setItem(row,1,new QTableWidgetItem(QString::fromStdString(guid)));
            tableWidget->setItem(row,2,new QTableWidgetItem(QString::fromStdString(password)));
            row++;
        }
    }
    for(int i=0;i<tableWidget->rowCount();i++)
    {
        tableWidget->item(i,0)->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
        tableWidget->item(i,0)->setTextColor(QColor(200,111,30)); //设置文字颜色
        tableWidget->item(i,1)->setFont(QFont("Helvetica")); //设置字体
        tableWidget->item(i,1)->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
        tableWidget->item(i,1)->setTextColor(QColor(200,111,30)); //设置文字颜色
        tableWidget->item(i,2)->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
        tableWidget->item(i,2)->setTextColor(QColor(200,111,30)); //设置文字颜色
    }
    tableWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    tableWidget->resizeColumnsToContents();
    tableWidget->resizeRowsToContents();
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    tableWidget->show();
}
