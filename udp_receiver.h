#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include <QMainWindow>
#include <qudpsocket.h>
#include <QThread>
#include <cinttypes>
#include <algorithm>

class UdpReceiver : public QObject {
    Q_OBJECT

private:
    QUdpSocket *udp_Socket;
    std::uint16_t port;

    QByteArray datagram;
    QByteArray datagram_header;

    QMap <std::uint8_t, QString> message_types = {
        {0, "common"},
        {2, "request"},
        {4, "file"},
        {8, "system"}
    };
    QMap <std::uint8_t, QString> system_types = {
        {1, "start"},
        {4, "filestart"},
        {2, "stop"}
    };

    bool request_status;
    QString message_type;
    QString system_type;
    std::uint16_t start_message_id;
    std::uint16_t filestart_message_id;
    std::uint16_t message_id;

    QMap <std::uint16_t, QByteArray> message_pool;
    QMap <std::uint16_t,QMap<std::uint16_t,QString>> filenames_pool;
    QMap <std::uint16_t, QMap<std::uint16_t,QByteArray>> files_pool;
    QMap <std::uint16_t, std::uint16_t> messages_counter;
    QVector<std::uint16_t> all_messages_pool;


    void Listen_Port(std::uint16_t port_local);
    void Translate_Header(std::uint8_t datagram_header);
    void Send_Answer(QHostAddress ip, std::uint16_t id);

private slots:

    void init_Thread();

signals:
    void to_Chat(QByteArray data, QHostAddress ip, QByteArray id);
    void to_Log(QByteArray data, QHostAddress ip);
    void to_File(QByteArray data, QString filename);
    void resend_Lost(QByteArray start_message_id, QHostAddress ip);

public:
    UdpReceiver(std::uint16_t from_port, QObject* parent = 0);
    ~UdpReceiver();

};

#endif // UDP_RECEIVER_H
