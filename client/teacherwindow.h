#ifndef TEACHERWINDOW_H
#define TEACHERWINDOW_H

#include <QMainWindow>
#include"db_client.h"
#include"file_client.h"
#include<QLabel>
#include<QSignalMapper>
namespace Ui {
  class TeacherWindow;
}

class TeacherWindow : public QMainWindow
{
  Q_OBJECT

public:
  //获取mapper信息并处理（槽函数）
  Q_SLOT void MapperReciver(QString title);
  //获取用户选择的上传课程（槽函数）
  //void GetUpCourse(QString course);
  //获取当前任务（槽函数）
  void GetTask();
  //下载文件（槽函数）
  void DownLoad();
  //上传文件（槽函数）
  void UpLoad();

  //重载构造函数
  TeacherWindow(SOCKET &sock,QSqlDatabase &db,QString username);

  explicit TeacherWindow(QWidget *parent = nullptr);
  ~TeacherWindow();

private:
  //用于多信号连接同一个槽
  QSignalMapper *mapper;
  //状态文本
  QLabel *logStat;

  //接收构造函数传参
  SOCKET sock;
  QSqlDatabase db;
  QString username;
  Ui::TeacherWindow *ui;

  //获取用户真实姓名
  QString GetRealName();

  //获取用户ID
  QString GetID();
};



#endif // TEACHERWINDOW_H
