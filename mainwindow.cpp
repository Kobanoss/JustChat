#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "udp_sender.h"
#include "udp_sender.cpp"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QWidget::setEnabled(true);
    ui->SendButton->setEnabled(true);
    ui->InputText->setEnabled(true);
    ui->FreqSlider->setEnabled(true);
    ui->SizeSlider->setEnabled(true);

    Set_Global_Vars();
}


MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::Set_Global_Vars() {
    package_size = 1;
    package_freq = 1;
    files_count = 0;
}


void MainWindow::on_SizeSlider_valueChanged(int package_size_local) {
    ui->SizeSpinBox->setValue((uint64_t)package_size_local);
}
void MainWindow::on_SizeSpinBox_editingFinished() {
    ui->SizeSlider->setValue((uint64_t)ui->SizeSpinBox->value());
    Set_Package_Size((uint64_t)ui->SizeSpinBox->value());
}
void MainWindow::on_SizeSlider_sliderReleased() {
    ui->SizeSpinBox->setValue((uint64_t)ui->SizeSlider->value());
    Set_Package_Size((uint64_t)ui->SizeSpinBox->value());
}
void MainWindow::Set_Package_Size(uint64_t package_size_local) {
    package_size = package_size_local;
     ui->LogBrowser->append(("Changed package size to "+QString::number(package_size_local)+'\n' ));
}


void MainWindow::on_FreqSlider_valueChanged(int package_freq_local){
    ui->FreqSpinBox->setValue((uint64_t)package_freq_local);
}
void MainWindow::on_FreqSpinBox_editingFinished() {
    ui->FreqSlider->setValue((uint64_t)ui->FreqSpinBox->value());
    Set_Package_Freq((uint64_t)ui->FreqSpinBox->value());
}
void MainWindow::on_FreqSlider_sliderReleased() {
    ui->FreqSpinBox->setValue((uint64_t)ui->FreqSlider->value());
    Set_Package_Freq((uint64_t)ui->FreqSpinBox->value());
}
void MainWindow::Set_Package_Freq(uint64_t package_freq_local) {
    package_freq = package_freq_local;
    ui->LogBrowser->append(("Changed package frequency to "+QString::number(package_freq_local)+'\n' ));
}



void MainWindow::Set_File_status() {
    ui->FileStatus->setText(QString::number(files_count) + " files added");
}


void MainWindow::on_SendButton_pressed() {
    QString message = ui->InputText->toPlainText();
    if (message.size() || files_count) {
        ui->ChatBrowser->append(message+((files_count>0)?("\n### Connected "+QString::number(files_count)+" files"):"\n"));
    }
    ui->InputText->clear();

    files_count = 0;
    Set_File_status();
////Testing
///
    uint16_t port = 10000;
    QString data = "test";
    QThread *sending_thread = new QThread();
    UdpSender * send = new UdpSender(package_size, package_freq, port, data);
    send->moveToThread(sending_thread);
    send->connect(sending_thread,SIGNAL(started()),send,SLOT(test()));
    sending_thread->start();
}


void MainWindow::on_FileButton_pressed() {
    files_count++;
    ui->LogBrowser->append("File added to message (total ="+QString::number(files_count)+")\n");
    Set_File_status();

}





