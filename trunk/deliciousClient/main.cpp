#include <QApplication>
#include <QMenuBar>

#include "mainwindow.h"
#include "mapview.h"
#include "bluetoothmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainwindow;

#if _WIN32_WCE
    mainwindow.showFullScreen();
#else
    mainwindow.resize(640, 480);
    mainwindow.show();
#endif

    return a.exec();
}
