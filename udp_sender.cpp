#include "udp_sender.h"

UdpSender::UdpSender(std::uint64_t max_size,std::uint64_t delay, std::uint16_t from_port,QByteArray rand_id,
                     bool need_request, bool need_request_lost,
                     QString message,QMap<QByteArray, QByteArray> files_data, QObject* parent) : QObject(parent) {
    qDebug() << ">Creating SENDING thread from main thread:" << QThread::currentThreadId();
    package_size = max_size;
    package_delay = delay;
    port = from_port;
    data_message = message.toUtf8();
    data_files = files_data;
    request_status = need_request;
    request_lost_status = need_request_lost;
    start_rand_id = rand_id;


    rand_generator.seed(time(0));
    sending_address = QHostAddress::Broadcast;
}

UdpSender::~UdpSender() {
}


void UdpSender::init_Thread() {
    qDebug() << ">Initialisation of SENDING thread:" << QThread::currentThreadId();
    udp_Socket = new QUdpSocket;
    Send_Data();

}


void UdpSender::Send_Data() {
    qDebug() << ">Starting Broadcast from port " << port << "\n";

    message_id = 0;
    Create_Message_Header("system", "start");
    Send_Package();

    if (data_message != nullptr) {

        local_start = 0;
        while (local_start <= std::uint64_t(data_message.size())) {
             usleep(package_delay*1000);

             Split_Data();
             Create_Message_Header("common");
             Send_Package();
             if (std::uint64_t(data_message.size()) < package_size) break;
        }
     }

     if (data_files.size() != 0) {
         for (auto filename: data_files.keys()) {
             filestart_rand_id = Create_Random_Id();

             data_file = data_files[filename];
             data_filename = filename;

             Create_Message_Header("system", "filestart");
             Send_Package();

             local_start = 0;
                 while (local_start <= std::uint64_t(data_file.size())) {
                     usleep(package_delay*1000);

                     Split_File_Data();
                     Create_Message_Header("file");
                     Send_Package();

                     if (std::uint64_t(data_file.size()) < package_size) break;
                 }
         }
     }

     Create_Message_Header("system", "stop");
     Send_Package();
}


void UdpSender::Split_Data() {
    package_data = data_message.mid(local_start, package_size);
    local_start += package_size;
}

void UdpSender::Split_File_Data() {
    package_data = data_file.mid(local_start, package_size);
    local_start += package_size;
}


void UdpSender::Create_Message_Header(QString type, QString system_type) {
    QByteArray message_id_bytes = (QByteArray::number(message_id));
    message_id_bytes.push_front("000000");
    message_id_bytes = message_id_bytes.right(5);

    message_header = QByteArray::number(16 + ((request_status && (type != "request"))?1:0) + message_types[type]);

    if (type == "common") {
        system_header = QByteArray::number(0) + start_rand_id + message_id_bytes;
        message_id++;
        return;
    }

    if (type == "system"){
        system_header = QByteArray::number(system_types[system_type]) + start_rand_id + message_id_bytes
                + ((system_type == "filestart")?filestart_rand_id:"") + ((system_type == "filestart")?data_filename:"");
        package_data = "";
        message_id++;
        return;
    }

    if (type == "file") {
        system_header = QByteArray::number(0) + start_rand_id + message_id_bytes
                + filestart_rand_id;
        message_id++;
        return;
    }

}


QByteArray UdpSender::Create_Random_Id(){
    std::uint16_t tmp = rand_generator() - 1;
    while (tmp < 10000) {tmp *=10;}
    return QByteArray::number(tmp);
}


void UdpSender::Send_Package() {
    udp_Socket->writeDatagram((message_header + system_header + package_data),sending_address, port);
    sending_address = QHostAddress::Broadcast;
    emit send_Status(start_rand_id, true);
}

void UdpSender::resend_Message(QByteArray id, QHostAddress sender_ip) {
    sending_address = QHostAddress::Broadcast;
    if (start_rand_id != id) return;
    sending_address = sender_ip;
    Send_Data();
}

void UdpSender::kill_Thread() {
}
