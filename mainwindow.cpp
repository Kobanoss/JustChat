#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "udp_sender.cpp"
#include "udp_receiver.cpp"
#include "udp_receiver.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    qRegisterMetaType<QHostAddress>("QHostAddress");

    QWidget::setEnabled(true);
    ui->SendButton->setEnabled(true);
    ui->InputText->setEnabled(true);
    ui->DelaySlider->setEnabled(true);
    ui->SizeSlider->setEnabled(true);

    Set_Global_Vars();

    QThread *receiving_thread = new QThread();
    UdpReceiver *receiver = new UdpReceiver(port);
    receiver->moveToThread(receiving_thread);

    connect(receiving_thread,SIGNAL(started()),receiver,SLOT(init_Thread()));
    connect(receiver, &UdpReceiver::to_Chat, this, &MainWindow::Data_To_Chat, Qt::BlockingQueuedConnection);
    connect(receiver, &UdpReceiver::to_File, this, &MainWindow::Data_To_File, Qt::BlockingQueuedConnection);
    connect(receiver, &UdpReceiver::to_Log, this, &MainWindow::Data_To_Log, Qt::BlockingQueuedConnection);
    connect(receiver, &UdpReceiver::resend_Lost, this, &MainWindow::resend_Lost, Qt::BlockingQueuedConnection);

    receiving_thread->start();

    ui->ChatBrowser->append("Welcome to JustChat!");

    timer = new QTimer();
    timer->setInterval(3000);
    connect(timer, SIGNAL(timeout()), this, SLOT(call_Resend()));
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::Set_Global_Vars() {
    package_size = 256;
    package_delay = 0;
    files_count = 0;
    files_data = {};
    port = 30000;
    request_status = false;
    rand_generator.seed(time(0));
    sending_threads = {};
}



void MainWindow::on_SizeSlider_valueChanged(int package_size_local) {
    ui->SizeSpinBox->setValue((std::uint64_t)package_size_local);
}
void MainWindow::on_SizeSpinBox_editingFinished() {
    ui->SizeSlider->setValue((std::uint64_t)ui->SizeSpinBox->value());
    Set_Package_Size((std::uint64_t)ui->SizeSpinBox->value());
}
void MainWindow::on_SizeSlider_sliderReleased() {
    ui->SizeSpinBox->setValue((std::uint64_t)ui->SizeSlider->value());
    Set_Package_Size((std::uint64_t)ui->SizeSpinBox->value());
}
void MainWindow::Set_Package_Size(std::uint64_t package_size_local) {
    package_size = package_size_local;
     ui->LogBrowser->append(("Changed package size to "+QString::number(package_size_local)+'\n' ));
}


void MainWindow::on_DelaySlider_valueChanged(int package_delay_local){
    ui->DelaySpinBox->setValue((std::uint64_t)package_delay_local);
}
void MainWindow::on_DelaySpinBox_editingFinished() {
    ui->DelaySlider->setValue((std::uint64_t)ui->DelaySpinBox->value());
    Set_Package_Delay((std::uint64_t)ui->DelaySpinBox->value());
}
void MainWindow::on_DelaySlider_sliderReleased() {
    ui->DelaySpinBox->setValue((std::uint64_t)ui->DelaySlider->value());
    Set_Package_Delay((std::uint64_t)ui->DelaySpinBox->value());
}
void MainWindow::Set_Package_Delay(std::uint64_t package_delay_local) {
    package_delay = package_delay_local;
    ui->LogBrowser->append(("Changed package sending delay to "+QString::number(package_delay_local)+'\n' ));
}



void MainWindow::Set_File_status() {
    ui->FileStatus->setText(QString::number(files_count) + " files added");
}


void MainWindow::on_SendButton_pressed() {
    QString message = ui->InputText->toPlainText();

    if (message.size() || files_count) {
        Call_Sending_Thread(message);
    }
    ui->InputText->clear();

    files_count = 0;
    Set_File_status();
}


void MainWindow::on_FileButton_pressed() {
    QString raw_filename = QFileDialog::getOpenFileName(this, "Open File", QString(), "*.*");
    QFileInfo file_info(raw_filename);
    filename = file_info.fileName().toUtf8();

    QFile opened_file(raw_filename);
    opened_file.open(QIODevice::ReadOnly);
    files_data[filename] = opened_file.readAll();

    files_count++;
    ui->LogBrowser->append("File added to message (total = "+QString::number(files_count)+")\n");
    Set_File_status();

}

void MainWindow::Call_Sending_Thread(QString message) {
    std::uint16_t tmp = rand_generator() - 1;
    while (tmp < 10000) {tmp *=10;}
    QByteArray rand_id = QByteArray::number(tmp);

    QThread *sending_thread = new QThread();
    UdpSender *send = new UdpSender(package_size, package_delay, port,rand_id, request_status, request_lost_status,
                                    message, files_data);
    sending_threads[rand_id] = send;
    status_sending_threads[rand_id] = false;
    send->moveToThread(sending_thread);


    connect(sending_thread, SIGNAL(started()), send, SLOT(init_Thread()));
    connect(send, &UdpSender::send_Status, this, &MainWindow::send_Status, Qt::QueuedConnection);

    sending_thread->start();
    files_data = {};
}

void MainWindow::resend_Lost(QByteArray start_message_id, QHostAddress ip) {
    if (timer->isActive()) timer->stop();


    lost_messages[start_message_id].append(ip);
    connect(this, &MainWindow::start_Resend,sending_threads[start_message_id], &UdpSender::resend_Message, Qt::QueuedConnection);

    timer->start();
}

void MainWindow::call_Resend() {

    qDebug() << "Threads status: " << status_sending_threads;
    qDebug() << "Lost undelivered messages: "<< lost_messages;

    for (auto id: status_sending_threads.keys()) {
        QMutableVectorIterator<QHostAddress> iter(lost_messages[id]);
        while(iter.hasNext()) {
            QHostAddress target_ip = iter.next();
            if (status_sending_threads[id]) {
                status_sending_threads[id] = false;
                emit start_Resend(id, target_ip);
                iter.remove();
            }
        }
        if (lost_messages[id].isEmpty()) lost_messages.remove(id);
    }
}

void MainWindow::send_Status(QByteArray start_rand_id,bool status){
    status_sending_threads[start_rand_id] = status;
}

void MainWindow::Data_To_Chat(QByteArray data, QHostAddress ip, QByteArray id) {
    ui->ChatBrowser->append("\n\n" + ip.toString().mid(7) + "::" + id + " >>\n\n" + QString(data));
}

void MainWindow::Data_To_File(QByteArray data, QString filename) {
    ui->ChatBrowser->append("Added file \"" + filename + "\" to \"./Downloads\" directory");
    QDir download_dir("./Downloads");
    if (!download_dir.exists()) {
        download_dir.mkpath("");
    }

    QFile file("./Downloads/" + filename);
    if (file.open(QIODevice::ReadWrite)) {
            file.write(data);
        }
    file.close();
}

void MainWindow::Data_To_Log(QByteArray data, QHostAddress ip) {
    ui->LogBrowser->append("Message with id #" + QString(data) + " delivery confirmed from " + ip.toString().mid(7) + "\n");
}


void MainWindow::on_requestDevileryCheckBox_stateChanged(int state) {
    request_status = (state == 0)?false:true;
}
