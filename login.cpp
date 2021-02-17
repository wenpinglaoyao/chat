#include "login.h"
#include "ui_login.h"
#include <QMessageBox>

Login::Login(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::Login)
{
    _ui->setupUi(this);
}

Login::~Login()
{
    delete _ui;
}

void Login::on_pushButton_clicked()
{
    if(_ui->lineEdit->text() == ""){
        QMessageBox::critical(0,tr("错误"),tr("您应该输入您的用户名再登录"));
        return;
    }

    Widget *user = new Widget(0,_ui->lineEdit->text());
    this->close();
    user->show();
}
