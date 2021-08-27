#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <QMainWindow>
#include <qudpsocket.h>
#include <QThread>
#include <cinttypes>
#include <unistd.h>
#include <map>
#include <random>

class UdpSender : public QObject {
    Q_OBJECT

private:
    QUdpSocket *udp_Socket;
    std::uint16_t port;

    QByteArray data_message;
    QByteArray data_file;
    QByteArray data_filename;
    QMap<QByteArray, QByteArray> data_files;

    std::uint64_t package_size;
    std::uint64_t package_delay;

    bool request_status;
    bool request_lost_status;

    uint16_t message_id;
    QByteArray start_rand_id;
    QByteArray filestart_rand_id;

    QByteArray message_header;
    QByteArray system_header;

    QMap <QString,std::uint8_t> message_types = {
        {"common", 0},
        {"request", 2},
        {"file", 4},
        {"system", 8}
    };
    QMap <QString,std::uint8_t> system_types = {
        {"start", 1},
        {"filestart", 4},
        {"stop", 2}
    };

    std::uint64_t local_start;

    QByteArray package_data;

    std::mt19937 rand_generator;

    QHostAddress sending_address;

    void Send_Data();

    void Split_Data();
    void Split_File_Data();

    void Create_Message_Header(QString type, QString system_type = nullptr);
    QByteArray Create_Random_Id();

    void Send_Package();

private slots:
    void init_Thread();
    void kill_Thread();

public slots:
    void resend_Message(QByteArray id, QHostAddress sender_ip);

signals:
    void send_Status(QByteArray start_rand_id, bool status);
public:
    UdpSender(std::uint64_t max_size,std::uint64_t delay, std::uint16_t port, QByteArray rand_id, bool need_request, bool need_request_lost,
              QString message = nullptr, QMap<QByteArray, QByteArray> files_data = {}, QObject* parent = 0);
    ~UdpSender();
};



#endif // UDP_SENDER_H
