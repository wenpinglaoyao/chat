#include "client.h"
#include "ui_client.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

Client::Client(QWidget *parent) :
    QDialog(parent), _ui(new Ui::Client)
{
    _ui->setupUi(this);
    _currentFileSize = _currentReceivedBytes = 0;
    _socket = new QTcpSocket(this); _file = new QFile();
    connect(_socket,&QIODevice::readyRead, this,&Client::slotReadFile);

    _inStream.setDevice(_socket);
    _inStream.setVersion(QDataStream::Qt_5_8);
}

//下面这个函数是槽函数，响应Widget类的信号函数【signalBeginReceive】
void Client::slotBeginReceive(QString serverAddr, quint16 port)
{
    _socket->abort();
    _socket->connectToHost(serverAddr,port);
}

void Client::slotReadFile()
{
    if(0 == _currentFileSize){
        _inStream>>_currentFileSize>>_fileName;
        _ui->progressBar->setMaximum(_currentFileSize);
        QFileInfo info(_fileName);
        QString fileName = info.fileName();
        _file->setFileName(fileName);
        if(!_file->open(QFile::WriteOnly)){
            qDebug()<<__FUNCTION__<<"无法打开文件!"<<_file->errorString();
            return;
        }
    }

    qint64 size = qMin(_socket->bytesAvailable(),_currentFileSize-_currentReceivedBytes);
    QByteArray array(size,0);
//    _inStream.readRawData(array.data(),size);
    array = _socket->readAll();
    _file->write(array);
    _currentReceivedBytes += size;

    if(_currentReceivedBytes == _currentFileSize){
        _ui->textBrowser->append(tr("成功接收文件【%1】！").arg(_fileName));
        reset();
    }

    _ui->progressBar->setValue(_currentReceivedBytes);
}
void Client::reset()
{
    _file->close();
    _currentFileSize = _currentReceivedBytes = 0;
    _fileName = "";
}
