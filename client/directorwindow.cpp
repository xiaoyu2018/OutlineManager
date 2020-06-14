#include "directorwindow.h"
#include "ui_directorwindow.h"
#include<QDebug>

DirectorWindow::DirectorWindow(QSqlDatabase &db,QString username) :ui(new Ui::DirectorWindow)
{
  ui->setupUi(this);
  //获取信息
  this->db=db;
  this->username=username;

  //设置状态栏标签
  logStat=new QLabel;
  logStat->setText("欢迎你 "+GetRealName(GetID(username)));
  ui->statusbar->addWidget(logStat);


  //按下退出按钮直接退出系统
  connect(ui->btn_exit,&QPushButton::clicked,[=](){this->close();});
  //查看分配状态
  connect(ui->btn_show,&QPushButton::clicked,this,&DirectorWindow::GetAllocation);
  //确认修改
  connect(ui->btn_confirm,&QPushButton::clicked,this,&DirectorWindow::SetPermission);

}

void DirectorWindow:: GetAllocation()
{
  //初始化下拉框
  ui->box_ID->clear();
  ui->box_course->clear();

  //获取全部课程
  QSqlQuery sQuery = QSqlQuery(db);
  //获取课程负责人ID
  QSqlQuery sQuery1 = QSqlQuery(db);
  //获取全部ID
  QSqlQuery sQuery2 = QSqlQuery(db);
  //确认ID是否为系主任
  QSqlQuery sQuery3 = QSqlQuery(db);

  QString courseName;
  QString ID;

  //构造查询字符串获取全部课程名
  QString str("SELECT CourseName FROM Course");
  sQuery.prepare(str);
  sQuery.exec();

  //构造字符串查找所有ID
  str="SELECT ID FROM Teacher";
  sQuery2.prepare(str);
  sQuery2.exec();




  //获取全部教师ID
  while (sQuery2.next())
    {
      //构造字符串确认身份
      str="SELECT Identity FROM Account WHERE ID = '";
      str+=sQuery2.value(0).toString();
      str+="'";
      //qDebug()<<str;
      sQuery3.prepare(str);
      sQuery3.exec();
      sQuery3.next();
      //qDebug()<<sQuery3.value(0).toBool();
      if(sQuery3.value(0).toBool()==false)
        ui->box_ID->addItem(sQuery2.value(0).toString());
    }


  //遍历
  for (int i=0;sQuery.next();i++)
    {
      courseName=sQuery.value(0).toString();
      ui->box_course->addItem(courseName);
      str="SELECT ID FROM Course WHERE CourseName = '";
      str+=courseName;
      str+="'";
      sQuery1.prepare(str);
      sQuery1.exec();
      sQuery1.next();
      ID=sQuery1.value(0).toString();
      ui->tableWidget->setRowCount(60);
      ui->tableWidget->setItem(i,0,new QTableWidgetItem(courseName));
      ui->tableWidget->setItem(i,1,new QTableWidgetItem(ID));
      ui->tableWidget->setItem(i,2,new QTableWidgetItem(GetRealName(ID)));
    }
  logStat->setText("已获取分配状态");
}

void DirectorWindow::SetPermission()
{
  //查询分配状态
  QSqlQuery sQuery = QSqlQuery(db);
  //更改权限
  QSqlQuery sQuery1 = QSqlQuery(db);

  //获取当前下拉框值
  QString course,ID,operation;
  course=this->ui->box_course->currentText();
  ID=this->ui->box_ID->currentText();
  operation=this->ui->box_permission->currentText();

  if(course.isNull())
    logStat->setText("请先查看当前分配状态");
  else
    {
      //判断课程是否已有负责人
      sQuery.prepare("SELECT Status FROM Course WHERE CourseName =?");
      sQuery.addBindValue(course);
      sQuery.exec();
      sQuery.next();
      if(operation=="增加权限")
        {
          if(sQuery.value(0).toBool()==true)
            {
              logStat->setText("课程已被分配，不可重复分配！");
              return;
            }
          sQuery1.prepare("update Course set Status=true,ID=? WHERE CourseName =?");
          sQuery1.addBindValue(ID);
          sQuery1.addBindValue(course);
          sQuery1.exec();
        }
      else
        {
          //无负责人不需修改
           if(sQuery.value(0).toBool()==false)
             goto end;
           sQuery1.prepare("update Course set Status=false,ID=NULL WHERE CourseName =?");
           sQuery1.addBindValue(course);
           sQuery1.exec();
        }
end:      logStat->setText("权限更改成功！");
    }
}

QString DirectorWindow::GetID(QString u_name)
{
  QSqlQuery sQuery = QSqlQuery(db);
  //构造查询字符串获取ID
  QString str("SELECT ID FROM Account WHERE UserName = '");
  str+=u_name;
  str+="'";
  sQuery.prepare(str);
  sQuery.exec();
  //移到第一条数据
  sQuery.next();
  return sQuery.value(0).toString();
}
QString DirectorWindow::GetRealName(QString ID)
{
  QSqlQuery sQuery = QSqlQuery(db);
  QString str=("SELECT TName FROM Teacher WHERE ID = '");
  str+=ID;
  str+="'";
  sQuery.prepare(str);
  sQuery.exec();
  sQuery.next();
  return sQuery.value(0).toString();
}











DirectorWindow::DirectorWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::DirectorWindow)
{
  ui->setupUi(this);
}

DirectorWindow::~DirectorWindow()
{
  delete ui;
}
