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
#endif
    //connect(m_ui->menuExit, SIGNAL(triggered()), this, SLOT(close()));
	//qDebug()<<this->m_ui->stackedWidget->widget(1)->size().width()<<endl;
	//qDebug()<<this->m_ui->stackedWidget->widget(1)->size().height()<<endl;

	navi = new mapview(NULL,this->m_ui->stackedWidget->widget(0)->size().width() ,this->m_ui->stackedWidget->widget(0)->size().height());
	int index = this->m_ui->stackedWidget->insertWidget(0,navi);
	qDebug()<<index<<endl;
	this->m_ui->stackedWidget->setCurrentWidget(navi);

	interfaceTransit_map();

	m_ui->centralwidget->setStyleSheet("background-image: url(:/Icons/Resources/images/background.png)");

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

void MainWindow::clearConnections()
{
	this->m_ui->actionA->disconnect();
	this->m_ui->actionB->disconnect();
	this->m_ui->actionC->disconnect();
	this->m_ui->actionD->disconnect();
	this->m_ui->actionE->disconnect();
	this->m_ui->actionL->disconnect();
	this->m_ui->actionR->disconnect();
	this->m_ui->actionPL->disconnect();
	this->m_ui->actionPR->disconnect();
}

void MainWindow::interfaceTransit_map()
{
	clearConnections();

	m_ui->toolButton_L->setText("-");
	m_ui->toolButton_R->setText("+");
	m_ui->pushButton_L->setText("Exit");
	m_ui->pushButton_R->setText("Lock map");

	m_ui->stackedWidget->setCurrentIndex(0);
	m_ui->lineEdit->setVisible(true);
	m_ui->toolButton_A->setVisible(true);
	m_ui->toolButton_B->setVisible(true);
	m_ui->toolButton_C->setVisible(true);
	m_ui->toolButton_D->setVisible(true);
	m_ui->toolButton_E->setVisible(true);

	connect(m_ui->actionR,SIGNAL(triggered()),this->navi,SLOT(zoomin()));
	connect(m_ui->actionL,SIGNAL(triggered()),this->navi,SLOT(zoomout()));
	connect(m_ui->actionA,SIGNAL(triggered()),this,SLOT(interfaceTransit_comment()));
	connect(m_ui->actionB,SIGNAL(triggered()),this,SLOT(interfaceTransit_favourite()));
	connect(m_ui->actionPL, SIGNAL(triggered()), this, SLOT(close()));
	
}


void MainWindow::interfaceTransit_comment()
{
	clearConnections();

	m_ui->toolButton_L->setText("lock");
	m_ui->toolButton_R->setText("delete");
	m_ui->pushButton_L->setText("Back");
	m_ui->pushButton_R->setText("Detail");

	m_ui->stackedWidget->setCurrentIndex(1);
	m_ui->lineEdit->setVisible(false);
	m_ui->toolButton_C->setVisible(false);
	m_ui->toolButton_D->setVisible(false);


	connect(m_ui->actionPL,SIGNAL(triggered()),this,SLOT(interfaceTransit_map()));
}


void MainWindow::interfaceTransit_favourite()
{
	clearConnections();

	m_ui->toolButton_L->setText("Archive");
	m_ui->toolButton_R->setText("share");
	m_ui->pushButton_L->setText("Back");
	m_ui->pushButton_R->setText("Detail");

	m_ui->stackedWidget->setCurrentIndex(2);
	m_ui->lineEdit->setVisible(false);
	m_ui->toolButton_A->setVisible(false);
	m_ui->toolButton_B->setVisible(false);
	m_ui->toolButton_C->setVisible(false);
	m_ui->toolButton_D->setVisible(false);
	m_ui->toolButton_E->setVisible(false);


	connect(m_ui->actionPL,SIGNAL(triggered()),this,SLOT(interfaceTransit_map()));
}