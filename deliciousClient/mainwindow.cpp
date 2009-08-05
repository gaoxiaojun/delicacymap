#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mapview.h"
#include "bluetoothmanager.h"

#include <QMenuBar>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
#if _WIN32_WCE
    menuBar()->setDefaultAction(m_ui->menuZoomOut);
#endif //
    connect(m_ui->menuExit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::BTHFind()
{
    bluetoothmanager &mgr = bluetoothmanager::GetInstance();
    mgr.Discover();
    QString list = QString("Local: ") + mgr.GetLocalDevice()->Name().c_str() + "\r\n";
    for(int i=0;i<mgr.NumOfDevices();++i)
    {
        bluetoothdevice* dev = mgr.GetDevice(i);

        list += dev->Name().c_str();
        list += "\r\n";
    }
    qDebug()<<list;
    mgr.SendToDevice(mgr[0], "kkk", 3);
}

void MainWindow::connectmenuevent()
{
    connect(m_ui->menuZoomOut, SIGNAL(triggered()), (mapview*)centralWidget(), SLOT(zoomout()));
    connect(m_ui->menuZoomIn, SIGNAL(triggered()), (mapview*)centralWidget(), SLOT(zoomin()));
    connect(m_ui->menuGPSLoc, SIGNAL(triggered()), (mapview*)centralWidget(), SLOT(whereami()));
    connect(m_ui->menuBTHfind, SIGNAL(triggered()), this, SLOT(BTHFind()));
}