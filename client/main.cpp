#include"logwindow.h"
#include <QApplication>
#include<QTextCodec>
/***************测试******************/
#include"teacherwindow.h"
#include"db_client.h"
#include"file_client.h"
#include"directorwindow.h"
/***************************************/

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());

  QApplication a(argc, argv);

  LogWindow logwindow;
  logwindow.show();
/********测试******************/
//  SOCKET s;
//  QSqlDatabase db;
//  ConnecT2Server(s);
//  ConnecT2db(db);
//  TeacherWindow t(s,db,"t1");
//  t.show();
/********测试******************/
//    QSqlDatabase db;
//    ConnecT2db(db);
//    DirectorWindow d(db,"d1");
//    d.show();
  return a.exec();
}
