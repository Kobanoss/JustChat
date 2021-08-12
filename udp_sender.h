#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <QMainWindow>
#include <qudpsocket.h>
#include <QThread>
#include <cinttypes>

class UdpSender : public QObject {
    Q_OBJECT
private:
    QUdpSocket *udp_Socket;
    uint16_t port;
    QString data_str;
    QByteArray data_file;
    QByteArray data_bytes;

    uint64_t package_size;
    uint64_t package_freq;

public:
    UdpSender(uint64_t max_size,uint64_t freq, uint16_t port,QString message,QByteArray file = nullptr, QObject* parent = 0);
    ~UdpSender();

public slots:

    void test();

    void init_thread();

//    void CreateDiagrams();

//    void SendDiagrams();

private:

};



#endif // UDP_SENDER_H
