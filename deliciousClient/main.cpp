#include <QApplication>
#include <QMenuBar>

#include "mainwindow.h"
#include "mapview.h"
#include "bluetoothmanager.h"
#undef abort
#include "ConnectionManager.h"

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

    //mapview navi(NULL, mainwindow.size().width(), mainwindow.size().height());
	//mainwindow.m_ui->stackedWidget->insertWidget(1,&navi);
    //mainwindow.setCentralWidget(&navi);
    //navi.setFocus();
    //mainwindow.connectmenuevent();

	//ConnectionManager connectionManager;

    return a.exec();
}
