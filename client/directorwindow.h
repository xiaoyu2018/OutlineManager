#ifndef DIRECTORWINDOW_H
#define DIRECTORWINDOW_H

#include <QMainWindow>
#include"db_client.h"
#include<QLabel>
namespace Ui {
  class DirectorWindow;
}

class DirectorWindow : public QMainWindow
{
  Q_OBJECT

public:
  //查看分配(槽函数)
  void GetAllocation();
  //设置权限(槽函数)
  void SetPermission();
  //重载构造函数
  DirectorWindow(QSqlDatabase &db,QString username);

  explicit DirectorWindow(QWidget *parent = nullptr);
  ~DirectorWindow();

private:
  //状态文本
  QLabel *logStat;

  //接收构造函数传参
  QSqlDatabase db;
  QString username;

  //获取用户真实姓名
  QString GetRealName(QString ID);

  //获取用户ID
  QString GetID(QString u_name);

  Ui::DirectorWindow *ui;
};

#endif // DIRECTORWINDOW_H
