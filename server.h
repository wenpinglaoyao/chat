#ifndef SERVER_H
#define SERVER_H
#include <QDialog>
#include <QTime>
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>
namespace Ui{
class Server;
}

class Server : public QDialog
{
    Q_OBJECT
public:
    explicit Server(QWidget *parent=0);
    ~Server() { delete _ui; }

private:
    Ui::Server *_ui;
    QQueue<QString> _fileQueue;
    qint64 _totalFileSize,_totalSendBytes,_currentFileSendBytes,_currentFileSize;
    const qint64 _blockSize=1024;
    QFile _file;
    QTcpServer *_server;
    QTcpSocket *_socket;
    QDataStream _outStream;

private slots:
    void slotOnNewConn();
    void slotOnWrite(qint64);

    void on_sCloseBtn_clicked();
    void on_sOpenBtn_clicked();
    void on_sSendBtn_clicked();

private:
    void reset();
    void startSend();


signals:
    void signalBeginSend();
};

#endif // SERVER_H
