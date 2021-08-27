#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <cinttypes>
#include <qudpsocket.h>
#include <random>
#include <udp_sender.h>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private slots:
    void on_SendButton_pressed();

    void on_SizeSlider_valueChanged(int value);
    void on_SizeSpinBox_editingFinished();
    void on_SizeSlider_sliderReleased();

    void on_DelaySlider_valueChanged(int value);
    void on_DelaySpinBox_editingFinished();
    void on_DelaySlider_sliderReleased();

    void on_FileButton_pressed();

    void on_requestDevileryCheckBox_stateChanged(int arg1);

    void resend_Lost(QByteArray start_message_id, QHostAddress ip);
    void send_Status(QByteArray start_rand_id, bool status);
    void call_Resend();

private:

    std::uint8_t files_count;
    std::uint64_t package_size;
    std::uint64_t package_delay;
    QMap<QByteArray, QByteArray> files_data;
    QByteArray filename;

    std::uint16_t port;

    std::mt19937 rand_generator;

    QMap<QByteArray, UdpSender*> sending_threads;
    QMap<QByteArray, bool> status_sending_threads;
    QMap<QByteArray, QVector<QHostAddress>> lost_messages;

    bool request_status;
    bool request_lost_status;

    QTimer *timer;

    void Set_Package_Size(std::uint64_t package_size_local);
    void Set_Package_Delay(std::uint64_t package_delay_local);

    void Set_Global_Vars();
    void Set_File_status();

    void Start_Receiving_Thread();
    void Start_Sending_Thread(QString message);


    void Data_To_Chat(QByteArray data, QHostAddress ip, QByteArray id);
    void Data_To_File(QByteArray data, QString filename);
    void Data_To_Log(QByteArray data, QHostAddress ip);

    void Call_Sending_Thread(QString message);


    Ui::MainWindow *ui;

signals:
    void start_Resend(QByteArray id, QHostAddress ip);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

};
#endif // MAINWINDOW_H
