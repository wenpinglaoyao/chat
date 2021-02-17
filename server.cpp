#include "server.h"
#include "ui_server.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QStandardPaths>

Server::Server(QWidget *parent) :
    QDialog(parent),_ui(new Ui::Server),_totalFileSize(0),_totalSendBytes(0),
    _currentFileSendBytes(0),_currentFileSize(0),_socket(nullptr)
{
    _ui->setupUi(this);
    _ui->sSendBtn->setEnabled(false);
    _ui->progressBar->setVisible(false);

    _server = new QTcpServer;
    connect(_server,&QTcpServer::newConnection, this,&Server::slotOnNewConn);
}
void Server::slotOnNewConn()
{
    _socket = _server->nextPendingConnection();
    connect(_socket,SIGNAL(bytesWritten(qint64)), this,SLOT(slotOnWrite(qint64)));

    _outStream.setDevice(_socket);
    _outStream.setVersion(QDataStream::Qt_5_8);

    _ui->progressBar->setVisible(true);
    reset();
}
void Server::reset()
{
    _file.close();
    _currentFileSendBytes = _currentFileSize = 0;


    if(_fileQueue.isEmpty()){
        _socket->close();
        _ui->sStatusLbl->setText(tr("文件全部发送成功，请让客户端检查一下！"));
    }else{
        _file.setFileName(_fileQueue.dequeue());//在这里提取出文件名队列中的第0个文件，绑定到类成员_file上
        this->startSend();
    }
}
void Server::startSend()
{
    if(!_file.open(QFile::ReadOnly)){
        qDebug()<<__FUNCTION__<<_file.errorString();
        return;
    }
    qint64 fileSize = _file.size();
    QString fileName = _file.fileName();
    _outStream<<fileSize<<fileName;
}
void Server::slotOnWrite(qint64 bytes)
{
    QByteArray array = _file.read(_blockSize);
    if(array.isEmpty()){
        QFileInfo info(_file);
        QString fileName = info.fileName();

        this->reset();
        return;
    }
    qint64 size = array.size();
    _outStream.writeRawData(array.constData(),size);

    _currentFileSendBytes += size;
    _totalSendBytes += size;
    _ui->progressBar->setMaximum(_totalFileSize);
    _ui->progressBar->setValue(_totalSendBytes);
}



void Server::on_sCloseBtn_clicked()
{
    _server->close();
   if(_socket)  _socket->close();
    _file.close();
    close();
}

void Server::on_sOpenBtn_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this,tr("服务器端提取文件们"),
                                                      QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if(!files.isEmpty()){
        _ui->sSendBtn->setEnabled(true);
        _ui->listWidget->addItems(files);
        for(int i=0;i<_ui->listWidget->count();i++){
            _fileQueue.append(_ui->listWidget->item(i)->text());
            QFileInfo info(_ui->listWidget->item(i)->text());
            _totalFileSize += info.size();
        }
    }
}

void Server::on_sSendBtn_clicked()
{
    if(!_server->listen(QHostAddress::Any,5555)){
        qDebug()<<__FUNCTION__<<"监听失败！："<<_server->errorString();
        return;
    }
    _ui->sStatusLbl->setText(tr("请通知对方接收您要发送的文件（们）"));
    _ui->sSendBtn->setEnabled(false);
    _ui->sOpenBtn->setEnabled(false);
    emit signalBeginSend();
    return;
}
