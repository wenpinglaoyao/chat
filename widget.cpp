#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QUdpSocket>
#include <QMessageBox>
#include <QScrollBar>
#include <QHostInfo>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>
#include <QKeyEvent>
Widget::~Widget()
{
    delete _ui;
}

Widget::Widget(QWidget *parent,QString username) :
    QWidget(parent), _ui(new Ui::Form), _username(username)
{
    _ui->setupUi(this);
    this->setWindowTitle(_username);
    _port = 11111;
    _udpSocket = new QUdpSocket;
    _udpSocket->bind(_port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(_udpSocket,SIGNAL(readyRead()),
            this,SLOT(processPending()));

    sendMsg(UserEnter);



    _client = new Client(this);
    connect(this,SIGNAL(signalBeginReceive(QString,quint16)), _client,SLOT(slotBeginReceive(QString,quint16)));
}
void Widget::sendMsg(MsgType type)
{
    QByteArray datagram;
    QDataStream out(&datagram,QIODevice::WriteOnly);
    out<<type<<_username;
    QString msg;
    switch(type){
    case Msg:
         msg = _ui->msgTxtEdit->toPlainText();
        _ui->msgTxtEdit->clear();
        _ui->msgTxtEdit->setFocus();
        out<<msg;
        break;
    case UserEnter:
        out<<this->getIP();
        break;
    case UserLeft:
        break;
    case FileName:
    {
        QString serverAddr = getIP();
        int row = _ui->usrTblWidget->currentRow();
        QString receiverName = _ui->usrTblWidget->item(0,0)->text();
        out<<serverAddr<<receiverName;
    }
        break;
    case Refuse:
        break;
    }

    _udpSocket->writeDatagram(datagram,datagram.length(),QHostAddress::Broadcast,_port);
}
void Widget::processPending()
{
    while(_udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(_udpSocket->pendingDatagramSize());
        _udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int type;
        in>>type;
        QString username,msg;
        QString time = QDateTime::currentDateTime().toString(("yyyy-MM-dd hh:mm:ss"));

        switch(type){
        case Msg:
            in>>username;
            _ui->msgBrowser->append(tr("用户[%1]说：").arg(username));
            in>>msg;
            _ui->msgBrowser->setTextColor(Qt::black);
            _ui->msgBrowser->append(msg);
            break;
        case UserEnter:
           { in>>username;
            QString ipAddr;
            in>>ipAddr;
            userEnter(username,ipAddr);}
            break;
        case UserLeft:
            in>>username;
            userLeft(username);
            break;
        case FileName:
        {
            QString serverName,serverAddr,receiverName;
            in>>serverName>>serverAddr>>receiverName;
            if(receiverName == _username){
                int btn = QMessageBox::information(this,tr("客户端询问框"),tr("文件已来，是否接收？"),
                                                   QMessageBox::Yes,QMessageBox::No);
                if(QMessageBox::Yes == btn){
                    emit signalBeginReceive(serverAddr,5555);
                    _client->show();
                }
            }
        }
            break;
        }
    }
}
void Widget::userEnter(QString username,QString ipAddr)
{
    bool isEmpty = _ui->usrTblWidget->findItems(username,Qt::MatchExactly).isEmpty();
    if(isEmpty){
        QTableWidgetItem *user = new QTableWidgetItem(username);
        QTableWidgetItem *ip     = new QTableWidgetItem(ipAddr);
        _ui->usrTblWidget->insertRow(0);
        _ui->usrTblWidget->setItem(0,0,user);
        _ui->usrTblWidget->setItem(0,1,ip);
        _ui->msgBrowser->setTextColor(Qt::green);
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        _ui->msgBrowser->append(tr("用户【%1】也在聊天室，登录时间：%2").arg(username).arg(time));
        _ui->usrNumLbl->setText(tr("当前在线人数：%1").arg(_ui->usrTblWidget->rowCount()));
        sendMsg(UserEnter);
    }
}

void Widget::userLeft(QString username)
{

    int row = _ui->usrTblWidget->findItems(username,Qt::MatchExactly).first()->row();
    _ui->usrTblWidget->removeRow(row);
    _ui->msgBrowser->setTextColor(Qt::gray);
    QString time = QDateTime::currentDateTime().toString(("hh:mm:ss"));
    _ui->msgBrowser->append(tr("用户[%1]于时间点%2离开了聊天室")
                            .arg(username).arg(time));
    _ui->usrNumLbl->setText(tr("当前在线人数：%1").arg(_ui->usrTblWidget->rowCount()));
}

QString Widget::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach(QHostAddress addr,list){
        if(QAbstractSocket::IPv4Protocol == addr.protocol())
            return addr.toString();
    }
    return 0;
}

void Widget::on_sendBtn_clicked()
{
    if("" == _ui->msgTxtEdit->toPlainText()){
        QMessageBox::warning(0,tr("警告"),tr("输入内容为空，无法发送"));
        return;
    }
    sendMsg(Msg);
}
void Widget::closeEvent(QCloseEvent *event){
    this->sendMsg(UserLeft);
    QWidget::closeEvent(event);
}

void Widget::on_sendTBtn_clicked()
{
    if(_ui->usrTblWidget->selectedItems().isEmpty()){
        QMessageBox::warning(0,tr("jinggao "),tr("还没有选择接收对象"),QMessageBox::Ok);
        return;
    }

    _server = new Server(this);
    connect(_server,SIGNAL(signalBeginSend()), this,SLOT(slotBeginSend()));
    _server->exec();
}

void Widget::slotBeginSend()
{
    sendMsg(FileName);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(Qt::Key_Return == event->key()){
        this->on_sendBtn_clicked();
    }
}
