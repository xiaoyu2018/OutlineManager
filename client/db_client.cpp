#include"db_client.h"

bool ConnecT2db(QSqlDatabase &db)
{
  //"D:\\SoBoring\\AdvancedProgram\\database\\sample.accdb"
  //数据库文件
  QString sDbNm ="database\\sample.accdb";
  db = QSqlDatabase::addDatabase("QODBC");//设置数据库驱动
  QString url = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
  db.setDatabaseName(url);//设置连接字符串
  db.setUserName("admin");//设置登陆数据库的用户名
  db.setPassword("2838896");//设置密码
  return db.open();
}

