#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <cinttypes>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_SendButton_pressed();

    void on_SizeSlider_valueChanged(int value);
    void on_SizeSpinBox_editingFinished();
    void on_SizeSlider_sliderReleased();

    void on_FreqSlider_valueChanged(int value);
    void on_FreqSpinBox_editingFinished();
    void on_FreqSlider_sliderReleased();

    void on_FileButton_pressed();

private:
    uint8_t files_count;
    uint64_t package_size;
    uint64_t package_freq;

    void Set_Package_Size(uint64_t package_size_local);
    void Set_Package_Freq(uint64_t package_freq_local);

    void Set_Global_Vars();
    void Set_File_status();


    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
