#include "teacherwindow.h"
#include "ui_teacherwindow.h"
#include"file_client.h"
#include"dialogt_upload.h"
#include<QDebug>
#include<QFileDialog>
#include<QDialogButtonBox>
#include<QFileInfo>
#include <QTextCodec>

//教师窗口主函数
TeacherWindow::TeacherWindow(SOCKET &sock,QSqlDatabase &db,QString username):ui(new Ui::TeacherWindow)
{

  ui->setupUi(this);
  //获取信息
  this->db=db;
  this->sock=sock;
  this->username=username;

  //设置状态栏标签
  logStat=new QLabel;
  logStat->setText("欢迎你 "+GetRealName());
  ui->statusbar->addWidget(logStat);

  //按下退出按钮直接退出系统
  connect(ui->btn_exit,&QPushButton::clicked,[=](){this->close();});
  //显示当前任务
  connect(ui->btn_show,&QPushButton::clicked,this,[=](){
      ui->stackedWidget->setCurrentIndex(0);
    });
  connect(ui->btn_show,&QPushButton::clicked,this,&TeacherWindow::GetTask);
  //上传大纲
  connect(ui->btn_upload,&QPushButton::clicked,this,&TeacherWindow::UpLoad);
  //下载大纲
  connect(ui->btn_download,&QPushButton::clicked,this,[=](){
      ui->stackedWidget->setCurrentIndex(1);
    });
  connect(ui->btn_download,&QPushButton::clicked,this,&TeacherWindow::DownLoad);
}


QString TeacherWindow::GetID()
{
  QSqlQuery sQuery = QSqlQuery(db);
  //构造查询字符串获取ID
  QString str("SELECT ID FROM Account WHERE UserName = '");
  str+=username;
  str+="'";
  sQuery.prepare(str);
  sQuery.exec();
  //移到第一条数据
  sQuery.next();
  return sQuery.value(0).toString();
}
QString TeacherWindow::GetRealName()
{
  QSqlQuery sQuery = QSqlQuery(db);
  QString ID=GetID();
  QString str=("SELECT TName FROM Teacher WHERE ID = '");
  str+=ID;
  str+="'";
  sQuery.prepare(str);
  sQuery.exec();
  sQuery.next();
  return sQuery.value(0).toString();
}

void TeacherWindow::GetTask()
{

  QSqlQuery sQuery = QSqlQuery(db);
  //构造查询字符串获取ID
  QString ID=GetID();
  //构造字符串获取负责课程
  QString str="SELECT CourseName FROM Course WHERE ID = '";
  str+=ID;
  str+="'";
  sQuery.prepare(str);
  sQuery.exec();
  //删除原列表元素
  ui->listWidget->clear();
  ui->listWidget->addItem(QString("已分配给我的课程："));
  while (sQuery.next())
    {
      ui->listWidget->addItem(sQuery.value(0).toString());
    }
  logStat->setText("已获取当前任务");
}

void TeacherWindow::UpLoad()
{
  QSqlQuery sQuery = QSqlQuery(db);
  //构造查询字符串获取ID
  QString ID=GetID();
  //构造字符串获取全部负责课程
  QString str="SELECT CourseName FROM Course WHERE ID = '";
  str+=ID;
  str+="'";
  sQuery.prepare(str);
  sQuery.exec();

  //文件对话框默认路径
  QString defaultPath="D:\\SoBoring\\AdvancedProgram\\projects1\\clientZero";

  //消息框获取课程名及选择版本
  DialogT_upload dlg;

  while(sQuery.next())
    dlg.c_box->addItem(sQuery.value(0).toString());
  if(dlg.c_box->count()==0)
    {
      logStat->setText("你还没有任何课程的上传权限");
      return;
    }

  else
    logStat->setText("请选择要上传大纲的对应课程");
  dlg.exec();

  //点击取消
  if(dlg.flag)
    {
      logStat->setText("已取消");
      return;
    }


  //获取用户点击的课程名
  QString coursName=dlg.c_box->currentText();
  //获取用户选择的版本(转为Qstring类型)
  QString version=QString::number( dlg.s_box->value());
  //上传至服务器
  if(!coursName.isNull()&&!coursName.isEmpty())
    {
      logStat->setText("请选择你要上传的大纲文件");
      QString file=QFileDialog::getOpenFileName(this,"选择上传文件",defaultPath,"(*.doc *.rtf *.pdf)");
      //路径中存在中文则无法上传
      if(file.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
        {
          logStat->setText("文件名及路径请不要包含中文");
          return;
        }
      //没选
      if(file.isNull()||file.isEmpty())
        {
          logStat->setText("已取消");
          return;
        }

      //转换绝对路径文件名格式
      std::string str1 = file.toStdString();
      const char* path = str1.c_str();

      //转换文件名格式
      QFileInfo fi;
      fi = QFileInfo(file);
      QString fileName = fi.suffix();  //获取文件名
//      QString header=GetRealName()+"-"+coursName+"-"+version+"-";//增加头
//      fileName=header+fileName;
      fileName = coursName+"-ver"+version+"."+fileName;
      std::string str2 = fileName.toStdString();
      const char* name = str2.c_str();


      if(UpLoadToSERVR(sock,path,name))
        {
          //修改数据库
          QSqlQuery sQuery = QSqlQuery(db);
          QString str("INSERT INTO Outline VALUES(");
          str+="'"+fileName+"'"+","+"'"+coursName+"')";
          sQuery.prepare(str);
          sQuery.exec();
          logStat->setText("上传成功！");
        }
      else
        logStat->setText("上传失败！");
    }

}


void TeacherWindow::DownLoad()
{
  QSqlQuery sQuery1 = QSqlQuery(db);
  QSqlQuery sQuery2 = QSqlQuery(db);
  //获得全部课程
  QString str="SELECT CourseName FROM Course";
  sQuery1.prepare(str);
  sQuery1.exec();

  //初始化树
  ui->treeWidget->clear();
  //初始化mapper
  mapper =new QSignalMapper;
  logStat->setText("请选择要下载的大纲");
  while(sQuery1.next())
    {

      QTreeWidgetItem *child=new QTreeWidgetItem(QStringList()<<sQuery1.value(0).toString());
      ui->treeWidget->addTopLevelItem(child);
      str="SELECT Title FROM Outline WHERE CourseName = '";
      str+=sQuery1.value(0).toString();
      str+="'";
      sQuery2.prepare(str);
      sQuery2.exec();
      while (sQuery2.next())
       {
          QTreeWidgetItem *c_child=new QTreeWidgetItem(QStringList()<<sQuery2.value(0).toString());
          child->addChild(c_child);
          QPushButton *btn = new QPushButton("下载");
          //设置映射参数
          connect(btn,SIGNAL(clicked()),mapper,SLOT(map()));
          mapper->setMapping(btn,sQuery2.value(0).toString());
          ui->treeWidget->setItemWidget(c_child,1,btn);
       }
    }
  //自动映射传递参数给处理函数
  connect(mapper,SIGNAL(mapped(QString)),this,SLOT(MapperReciver(QString)));
}



void TeacherWindow:: MapperReciver(QString title)
{
  qDebug()<<title;
  std::string str = title.toStdString();
  const char* name = str.c_str();

  if(DownLoadFromSERVR(sock,name))
    logStat->setText("下载成功！");
  else
    logStat->setText("下载失败！");
}




TeacherWindow::TeacherWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::TeacherWindow)
{
  ui->setupUi(this);
}

TeacherWindow::~TeacherWindow()
{
  delete ui;
}

