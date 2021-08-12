#include "udp_sender.h"

UdpSender::UdpSender(uint64_t max_size,uint64_t freq, uint16_t from_port,QString message,QByteArray file, QObject* parent) : QObject(parent) {
    qDebug() << ">Creating thread:" << QThread::currentThreadId();
    package_size = max_size;
    package_freq = freq;
    port = from_port;
    data_str = message;
    data_file = file;
}

UdpSender::~UdpSender() {

}


void UdpSender:: test() {
    qDebug() << "test";
    init_thread();
    qDebug() << "testq";
}


void UdpSender::init_thread() {
    udp_Socket = new QUdpSocket;
    udp_Socket->bind(port);
    // sender_socket->connect() TODO: Connect slot to send
    qDebug() << ">Sending UDP data from port " << port << "\n";
}
