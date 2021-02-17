#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "widget.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Login *_ui;
};

#endif // LOGIN_H
