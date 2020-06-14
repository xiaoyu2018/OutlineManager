#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include<QPushButton>


namespace Ui {
  class LogWindow;
}

class LogWindow : public QMainWindow
{
  Q_OBJECT

public:


  explicit LogWindow(QWidget *parent = nullptr);
  ~LogWindow();

private:

  //登陆进程状态
  QLabel *logStat;
  Ui::LogWindow *ui;

  //用户登录
  void LogIn();
  //连接数据库及服务器
  bool preConnect();
  //切换菜单
  void switchMenu(int mode);
  //检查身份
  int checkIdentity();
};

#endif // LOGWINDOW_H
