#include "mainwindow.h"
#include "udp_sender.h"
#include <QApplication>




int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
