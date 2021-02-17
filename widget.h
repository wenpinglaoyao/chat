#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "server.h"
#include "client.h"
namespace Ui {
class Form;
}
class QUdpSocket;
class Server;
enum MsgType{Msg=60, UserEnter, UserLeft, FileName, Refuse};

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent=0,QString username=0);
    ~Widget();

    QString getIP();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected slots:
    void processPending();

private slots:
    void on_sendBtn_clicked();
    void on_sendTBtn_clicked();

    void slotBeginSend();

private:
    void sendMsg(MsgType type);
    void userEnter(QString username,QString ip=0);
    void userLeft(QString username);


private:
    Ui::Form *_ui;
    QUdpSocket *_udpSocket;
    qint16 _port;
    QString _username,_pathName,_receiveName,fileName;
    Server *_server;
    Client *_client;

signals:
    void signalBeginReceive(QString,quint16);
};

#endif // WIDGET_H
