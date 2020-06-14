#ifndef DIALOGT_UPLOAD_H
#define DIALOGT_UPLOAD_H

#include <QDialog>
#include<QComboBox>
#include<QSpinBox>
namespace Ui {
  class DialogT_upload;
}

class DialogT_upload : public QDialog
{
  Q_OBJECT

public:
  //计数框（选版本）
  QSpinBox  *s_box;
  //下拉框(选课程)
  QComboBox *c_box;
  //取消标志位
  bool flag=false;

  explicit DialogT_upload(QWidget *parent = nullptr);
  ~DialogT_upload();

private:
  Ui::DialogT_upload *ui;

};

#endif // DIALOGT_UPLOAD_H
