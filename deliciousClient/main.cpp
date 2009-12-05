#include <QApplication>
#include <QMenuBar>
#include <QDialog>

#include "mainwindow.h"
#include "mapview.h"
#include "bluetoothmanager.h"
#include "LoginWindow.h"

Session* TryLogin();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Session* s = TryLogin();
    if (s)
    {
        MainWindow mainwindow( s );

#if _WIN32_WCE
        mainwindow.showFullScreen();
#else
        //mainwindow.resize(480, 600);
        mainwindow.show();
#endif

        return a.exec();
    }
    else
        return 0;
}

Session* TryLogin()
{
    LoginWindow login;
#if _WIN32_WCE
    login.showFullScreen();
#endif
    login.exec();
    return login.result() == QDialog::Accepted ? login.getSession() : NULL;
}
