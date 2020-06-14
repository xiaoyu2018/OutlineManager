#include "logwindow.h"
#include "ui_logwindow.h"
#include"file_client.h"
#include"db_client.h"
#include<QMessageBox>
#include<QString>
#include<QDebug>
#include<teacherwindow.h>
#include"directorwindow.h"

//客户端套接字
SOCKET sock;
//数据库实例
QSqlDatabase db;
//首次连接标识
bool isFirstConnec=true;

//主窗口构造函数（主函数）
LogWindow::LogWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::LogWindow)
{
  ui->setupUi(this);
  logStat=new QLabel;
  logStat->setText("欢迎登录");
  ui->statusbar->addWidget(logStat);

  //按下退出按钮直接退出系统
  connect(ui->exitbtn,&QPushButton::clicked,[=](){this->close();});

  //登录按钮信号绑定至登录槽
  connect(ui->logInBtn,&QPushButton::clicked,this,&LogWindow::LogIn);

}

void LogWindow:: LogIn()
{
  //判断连接是否成功
  if(isFirstConnec)
    {
      if(!preConnect())
        return;//未成功直接返回
      isFirstConnec=false;
    }
  int identity;
  //判断用户名密码是否正确
  if((identity=checkIdentity())==-1)
    {
       QMessageBox::critical(this,"通知","用户名或密码错误");
       return;
    }

  //登录成功，按身份创建不同窗口
  switchMenu(identity);
}
int LogWindow::checkIdentity()
{
  //声明数据库操作对象
  QSqlQuery sQuery = QSqlQuery(db);
  //查询用户表
  QString strSqlText("SELECT * FROM Account");
  //提交sql语句并接受返回值
  sQuery.prepare(strSqlText);
  sQuery.exec();
  //依次取出每一条记录并与用户输入比较
  while(sQuery.next())
    {
      if(ui->lineEdit_un->text()==sQuery.value("UserName").toString()&&
         ui->lineEdit_pw->text()==sQuery.value("PassWord").toString())
        {
          return sQuery.value("Identity").toInt();
        }
    }
  //未找到用户或密码错误返回-1
  return -1;
}
void LogWindow::switchMenu(int mode)
{
  //隐藏登录窗口
  this->hide();
  if(mode==1)
    {
      DirectorWindow *d=new DirectorWindow(db,ui->lineEdit_un->text());
      d->show();
    }
  else
    {
      TeacherWindow *t =new TeacherWindow(sock,db,ui->lineEdit_un->text());
      t->show();

    }
}

//连接服务器及数据库
bool LogWindow:: preConnect()
{
  //连接至文件服务器
  logStat->setText("连接服务器中...");
  if(!ConnecT2Server(sock))
    {
      logStat->setText("连接服务器失败！");
      return false;
    }
  if(!ConnecT2db(db))
    {
      logStat->setText("连接数据库失败！");
      return false;
    }
    logStat->setText("连接服务器成功！");
  return  true;
}

//析构函数
LogWindow::~LogWindow()
{
  //断开数据库连接并清除套接字
  db.close();
  WSACleanup();
  delete ui;
}
