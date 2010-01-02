#include <QApplication>
#include <QMenuBar>
#include <QDialog>
#include <QtDebug>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"
#include "mapview.h"
#include "bluetoothmanager.h"
#include "LoginWindow.h"
#include "qnetworksession.h"
#include "qnetworkconfigmanager.h"
#include "qnetworkconfiguration.h"

QTM_USE_NAMESPACE
Session* TryLogin();

void myMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1\n").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1\n").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1\n").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1\n").arg(msg);
        abort();
    }
    QFile outFile("e:\\log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

void openNetwork()
{
        QNetworkConfigurationManager manager;
        const bool canStartIAP = (manager.capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces);
        QList<QNetworkConfiguration> cfgs = manager.allConfigurations();
        for (int i=0;i<cfgs.size();++i)
        {
            if (cfgs[i].type() == QNetworkConfiguration::UserChoice)
            {
                QNetworkSession* network = new QNetworkSession(cfgs[i]);
                network->open();
                break;
            }
        }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMsgHandler(myMessageHandler);
    Session* s = TryLogin();
    qDebug()<<"start.";
    //openNetwork();
    if (s)
    {
        MainWindow mainwindow( s );

#if _WIN32_WCE || defined(Q_OS_SYMBIAN)
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
