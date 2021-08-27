#include "udp_receiver.h"

UdpReceiver::UdpReceiver(std::uint16_t from_port, QObject* parent) : QObject(parent) {
    qDebug() << ">Creating RECEVING thread from main thread:" << QThread::currentThreadId();
    port = from_port;
    datagram = nullptr;
    all_messages_pool = {};
}

UdpReceiver::~UdpReceiver() {
}


void UdpReceiver::init_Thread() {
    qDebug() << ">Initialisation of RECEIVING thread:" << QThread::currentThreadId();
    udp_Socket = new QUdpSocket;
    Listen_Port(port);

    udp_Socket->connect(udp_Socket, &QUdpSocket::readyRead, [&]() {
       if (udp_Socket->hasPendingDatagrams()) {

           QHostAddress sender_ip;
           std::uint16_t sender_port;

           datagram.resize(udp_Socket->pendingDatagramSize());
           udp_Socket->readDatagram(datagram.data(), datagram.size(), &sender_ip, &sender_port);

           qDebug() << "Sender IP: " << sender_ip;
           qDebug() << "Sender PORT: " << sender_port;
           qDebug() << "Size of datagram: " << datagram.size();

           if (datagram.data()[0] != false){
           Translate_Header(datagram.left(3).toUInt());
           start_message_id = datagram.mid(3,5).toUInt();
           if (std::find(all_messages_pool.begin(), all_messages_pool.end(), start_message_id) != all_messages_pool.end())
               goto end_of_read;
           message_id = datagram.mid(8,5).toUInt();

           qDebug() << "Request status: " << request_status;
           qDebug() << "Start message ID: " << start_message_id;
           qDebug() << "Message ID: " << message_id;

           if (message_type == "common") {
               qDebug() << "Message type: " << message_type;
               message_pool[start_message_id].append(datagram.mid(13));

               if (request_status) messages_counter[start_message_id] += 1;
               goto end_of_read;
           }

           if (message_type == "request") {
               qDebug() << "Message type: " << message_type;
               emit to_Log(QByteArray::number(start_message_id), sender_ip);

               goto end_of_read;
           }

           if (message_type == "system"){
               qDebug() << "Message type: " << message_type << system_type;

               if (system_type == "start") {
                   message_pool[start_message_id] = "";
                   files_pool[start_message_id] = {};
                   filenames_pool[start_message_id] = {};

                   if (request_status) messages_counter[start_message_id] = 1;
                   goto end_of_read;
               }
               if (system_type == "filestart") {
                   filestart_message_id = datagram.mid(13,5).toUInt();
                   files_pool[start_message_id][filestart_message_id] = "";
                   filenames_pool[start_message_id][filestart_message_id] = datagram.mid(18);

                   if (request_status) messages_counter[start_message_id] += 1;
                   goto end_of_read;
               }

               if (system_type == "stop") {
                   srand(time(NULL));
                   message_id += rand()%2;
                   if (request_status && (messages_counter[start_message_id] == message_id)) {
                       udp_Socket->writeDatagram("180" + QByteArray::number(start_message_id), sender_ip, port);
                   }
                   else if (request_status && messages_counter[start_message_id] != message_id) {
                       message_pool.remove(start_message_id);
                       files_pool.remove(start_message_id);
                       filenames_pool.remove(start_message_id);
                       messages_counter.remove(start_message_id);

                       udp_Socket->writeDatagram("190" + QByteArray::number(start_message_id), sender_ip, port);
                       goto end_of_read;
                   }

                   all_messages_pool.append(start_message_id);
                   emit to_Chat(message_pool[start_message_id], sender_ip, QByteArray::number(start_message_id));
                   for (auto file:files_pool[start_message_id].keys()) {
                       emit to_File(files_pool[start_message_id][file], filenames_pool[start_message_id][file]);
                   }

                   message_pool.remove(start_message_id);
                   files_pool.remove(start_message_id);
                   filenames_pool.remove(start_message_id);
                   messages_counter.remove(start_message_id);

                   goto end_of_read;
               }
           }

           if (message_type == "file") {
               qDebug() << "Message type: " << message_type;
               filestart_message_id = datagram.mid(13,5).toUInt();
               files_pool[start_message_id][filestart_message_id].append(datagram.mid(18));

               if (request_status) messages_counter[start_message_id] += 1;
               goto end_of_read;
           }

           if (message_type == "lost") {
               qDebug() << "Message type: " << message_type;
               emit resend_Lost(QByteArray::number(start_message_id), sender_ip);
           }

           end_of_read:;
           qDebug() << "\n";
       }
       }
    });
}


void UdpReceiver::Translate_Header(std::uint8_t message_header) {
    std::uint8_t system_status = message_header % 10;
    message_header /= 10;

    (message_header % 2 == 1)?request_status = true:request_status = false;
    if (system_status != 0) {
        message_type = "system";
        system_type = system_types[system_status];
        return;
    }
    if (request_status) message_header -= 1;
    message_header -= 16;
    message_type = message_types[message_header];
    if (message_type == "request" && request_status) {
        message_type = "lost";
        request_status = false;
    }
}


void UdpReceiver::Listen_Port(std::uint16_t port_local) {
    udp_Socket->bind(port_local,QUdpSocket::ShareAddress);
}


