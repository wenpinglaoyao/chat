#ifndef CLIENT_H
#define CLIENT_H
#include <QDialog>
#include <QHostAddress>
#include <QFileDialog>
#include <QFile>
#include <QTime>
#include <QTcpSocket>

namespace Ui { class Client; }

class Client : public QDialog
{
    Q_OBJECT
public:
    explicit Client(QWidget *parent=0);
    ~Client(){ delete _ui;}

private:
    Ui::Client *_ui;
    quint16 _port;
    qint64 _currentFileSize,_currentReceivedBytes;
    QTcpSocket *_socket;
    QFile *_file;
    QString _fileName;
    QDataStream _inStream;
    QByteArray _array;

    void reset();

private slots:
    void slotBeginReceive(QString serverAddr, quint16 port);
    void slotReadFile();

};

#endif // CLIENT_H
