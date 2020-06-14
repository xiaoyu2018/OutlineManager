#include "dialogt_upload.h"
#include "ui_dialogt_upload.h"
#include<QDialogButtonBox>
DialogT_upload::DialogT_upload(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogT_upload)
{
  ui->setupUi(this);
  c_box=ui->comboBox;
  ui->spinBox->setValue(1);
  s_box=ui->spinBox;
  connect(ui->buttonBox,&QDialogButtonBox::rejected,this,[=](){flag=true;});
}

DialogT_upload::~DialogT_upload()
{
  delete ui;
}
