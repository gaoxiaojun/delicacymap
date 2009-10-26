#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mapview.h"
#include "bluetoothmanager.h"
#include "CustomEvents.h"
#include "QTProtobufWaitResponse.h"
#include "..\protocol-buffer-src\MapProtocol.pb.h"

#include <QMenuBar>
#include <QDebug>

using namespace ProtocolBuffer;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
#if _WIN32_WCE
    //menuBar()->setDefaultAction(m_ui->menuZoomOut);
#endif
    //connect(m_ui->menuExit, SIGNAL(triggered()), this, SLOT(close()));
	//qDebug()<<this->m_ui->stackedWidget->widget(1)->size().width()<<endl;
	//qDebug()<<this->m_ui->stackedWidget->widget(1)->size().height()<<endl;

	navi = new mapview(this);
	int index = this->m_ui->stackedWidget->insertWidget(0,navi);
	qDebug()<<index<<endl;
	this->m_ui->stackedWidget->setCurrentWidget(navi);

	interfaceTransit_map();

	connect(m_ui->testRPC,SIGNAL(clicked()),this,SLOT(TestRPC()));
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

void MainWindow::customEvent( QEvent *e )
{
	QMainWindow::customEvent(e);
	ProtobufDataEvent *pe = (ProtobufDataEvent*)e;
	QTextEdit *text = m_ui->textEdit;
	switch (e->type())
	{
	case ProtobufDataEvent::RestaurantListRecv:
		{
			ProtocolBuffer::RestaurantList* rlist = (RestaurantList*)pe->data;
			for (int i=0;i<rlist->restaurants_size();++i)
			{
				text->append(QString("name=") + QString::fromUtf8(rlist->restaurants(i).name().c_str()) + QString(";RID=") + 
					QString::number(rlist->restaurants(i).rid()) + QString("\n"));
			}
			delete rlist;
			break;
		}
	case ProtobufDataEvent::CommentListRecvs:
		{
			CommentList *clist = (CommentList*)pe->data;
			for (int i=0;i<clist->comments_size();++i)
			{
				text->append(QString("content=") + QString::fromUtf8(clist->comments(i).content().c_str()) + QString(";UID=") + 
					QString::number(clist->comments(i).uid()) + QString("\n"));
			}
			delete clist;
		}
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

void MainWindow::TestRPC()
{
	QTextEdit* textbox = m_ui->textEdit;
	QTProtobufWaitResponse synccall;
	ProtocolBuffer::RestaurantList rlist;
	CommentList clist;
	Comment c;
	ProtocolBuffer::Query query;

	// fill all parameters
	query.mutable_area()->mutable_southwest()->set_latitude(0.0);
	query.mutable_area()->mutable_southwest()->set_longitude(0.0);
	query.mutable_area()->mutable_northeast()->set_latitude(10000.0);
	query.mutable_area()->mutable_northeast()->set_longitude(10000.0);
	query.set_level(8);
	query.set_n(1000);
	query.set_rid(1);
	query.set_uid(1);
	query.set_msg("test msg");

	textbox->append("Test Begin. Connected to 127.0.0.1\n");
	//GetRestaurants
	textbox->append("Restaurants found:\n");
	connman.GetRestaurants(&query, &rlist, synccall.getClosure());
	synccall.wait();
	for (int i=0;i<rlist.restaurants_size();++i)
	{
		textbox->append(QString("name=") + QString::fromUtf8(rlist.restaurants(i).name().c_str()) + QString(";RID=") + 
			QString::number(rlist.restaurants(i).rid()) + QString("\n"));
	}

	//GetLastestCommentsOfRestaurant
	textbox->append("Latest comment for RID=1");
	connman.GetLastestCommentsOfRestaurant(&query, &clist, synccall.getClosure());
	synccall.wait();
	for (int i=0;i<clist.comments_size();++i)
	{
		textbox->append(QString("content=") + QString::fromUtf8(clist.comments(i).content().c_str()) + QString(";UID=") + 
			QString::number(clist.comments(i).uid()) + QString("\n"));
	}
	

	textbox->append("Latest comment for UID=1");
	connman.GetLastestCommentsByUser(&query, &clist, synccall.getClosure());
	synccall.wait();
	for (int i=0;i<clist.comments_size();++i)
	{
		textbox->append(QString("content=") + QString::fromUtf8(clist.comments(i).content().c_str()) + QString(";UID=") + 
			QString::number(clist.comments(i).uid()) + QString("\n"));
	}

	textbox->append("Adding new comment.");
	connman.AddCommentForRestaurant(&query, &c, synccall.getClosure());
	synccall.wait();
	textbox->append(QString("content=") + QString::fromUtf8(c.content().c_str()) + QString(";UID=") + 
		QString::number(c.uid()) + QString("\n"));
}

void MainWindow::postEvent( ProtobufDataEvent* e )
{
	QApplication::postEvent(this, e);
}