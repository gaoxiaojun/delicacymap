#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bluetoothmanager.h"
#include "QTProtobufWaitResponse.h"
#include "MapController.h"
#include "Session.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MapDecorators.h"
#include "OfflineMap/MapServices.h"
#include "OfflineMap/GeoCoord.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QDebug>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

using namespace ProtocolBuffer;
QTM_USE_NAMESPACE

struct GeoCodeResultPair
{
    GeoPoint first, second;
    bool firstSet, secondSet;
};

MainWindow::MainWindow(Session *s, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    session(NULL)
{
    m_ui->setupUi(this);

#if _WIN32_WCE
    //menuBar()->setDefaultAction(m_ui->menuZoomOut);
#endif
    //connect(m_ui->menuExit, SIGNAL(triggered()), this, SLOT(close()));
	//qDebug()<<this->m_ui->stackedWidget->widget(1)->size().width()<<endl;
	//qDebug()<<this->m_ui->stackedWidget->widget(1)->size().height()<<endl;

    navi = new MapViewBase;
    navi->setDecorator(new MoveDecorator(navi));
    navi->insertDecorator(new ZoomDecorator(navi));
    navi->insertDecorator(new DownloadDecorator(navi));
//     CrossDecorator *crossDecorator = new CrossDecorator(navi);
//     connect(crossDecorator, SIGNAL(stateChanged()), navi, SLOT(repaint()));
//     navi->appendDecorator(crossDecorator);
//     connect(&imageCache, SIGNAL(imageChanged()), navi, SLOT(repaint()));
//     CoordsDecorator* coordsDecorator = new CoordsDecorator(navi);
//     connect(coordsDecorator, SIGNAL(stateChanged()), navi, SLOT(repaint()));
//     navi->appendDecorator(coordsDecorator);

    imageCache.setDownloader(&downloader);
    imageCache.setCacheDBPath("tiles.map");
    navi->setCache(&imageCache);

    controller = new MapController;
    controller->setMapView(navi);
    controller->setLocationSource(QGeoPositionInfoSource::createDefaultSource(this));
    connect(navi, SIGNAL(boundsChange(const GeoBound&)), controller, SLOT(MapViewBoundsChange(const GeoBound&)));
    connect(controller, SIGNAL(currentLocationUpdate(const GeoPoint&)), navi, SLOT(setSelfLocation(const GeoPoint&)));
    connect(controller, SIGNAL(SysMsgRequestRouting(int, QString, QString)), this, SLOT(handleRequestRouting(int, const QString&, const QString&)));

    navi->setZoomLevel(15);
    navi->setGeoCoords(GeoCoord(39.96067508327288), GeoCoord(116.35796070098877));

    svc = new MapServices;
    connect(svc, SIGNAL(RoutingResult(QList<GeoPoint>, void*)), controller, SLOT(AddEditingRouteInFavorOf(const QList<GeoPoint>&, void*)));

	int index = this->m_ui->stackedWidget->insertWidget(0,navi);
	qDebug()<<index<<endl;
	this->m_ui->stackedWidget->setCurrentWidget(navi);

    changeSession(s);

	interfaceTransit_map();
}

MainWindow::~MainWindow()
{
    delete navi;
    delete m_ui;
    delete controller;
    delete svc;
}

void MainWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        QMainWindow::changeEvent(e);
        break;
    }
}

void MainWindow::BTHFind()
{
/*    bluetoothmanager &mgr = bluetoothmanager::GetInstance();
    mgr.Discover();
    QString list = QString("Local: ") + mgr.GetLocalDevice()->Name().c_str() + "\r\n";
    for(int i=0;i<mgr.NumOfDevices();++i)
    {
        bluetoothdevice* dev = mgr.GetDevice(i);

        list += dev->Name().c_str();
        list += "\r\n";
    }
    qDebug()<<list;
    mgr.SendToDevice(mgr[0], "kkk", 3);*/
}

void MainWindow::changeSession( Session *s )
{
    if (session)
    {
        disconnect(&session->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), this, SLOT(printMessage(const ProtocolBuffer::DMessage*)));
        disconnect(&session->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), controller, SLOT(HandleSystemMessages(const ProtocolBuffer::DMessage*)));
    }
    session = s;
    //navi->changeSession(s);
    if (s)
    {
        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), this, SLOT(printMessage(const ProtocolBuffer::DMessage*)));
        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), controller, SLOT(HandleSystemMessages(const ProtocolBuffer::DMessage*)));
    }
    controller->setSession(s);
}

Session* MainWindow::getSession()
{
    return session;
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

	//connect(m_ui->actionR,SIGNAL(triggered()),this->navi,SLOT(zoomin()));
	//connect(m_ui->actionL,SIGNAL(triggered()),this->navi,SLOT(zoomout()));
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
    m_ui->tabWidget->setCurrentIndex(1);
    m_ui->lineEdit->setVisible(false);
    m_ui->toolButton_C->setVisible(false);
    m_ui->toolButton_D->setVisible(false);

    connect(m_ui->actionPL,SIGNAL(triggered()),this,SLOT(interfaceTransit_map()));
}


void MainWindow::interfaceTransit_favourite()
{
	qDebug()<<"click"<<endl;
	//navi->page()->mainFrame()->evaluateJavaScript(QString("alert('aa');"));
// 	navi->page()->mainFrame()->evaluateJavaScript(QString("removeRestaurant(1);"));
// 	navi->page()->mainFrame()->evaluateJavaScript(QString("removeRestaurant(2);"));



	//clearConnections();

	//m_ui->toolButton_L->setText("Archive");
	//m_ui->toolButton_R->setText("share");
	//m_ui->pushButton_L->setText("Back");
	//m_ui->pushButton_R->setText("Detail");

	//m_ui->stackedWidget->setCurrentIndex(2);
	//m_ui->lineEdit->setVisible(false);
	//m_ui->toolButton_A->setVisible(false);
	//m_ui->toolButton_B->setVisible(false);
	//m_ui->toolButton_C->setVisible(false);
	//m_ui->toolButton_D->setVisible(false);
	//m_ui->toolButton_E->setVisible(false);


	//connect(m_ui->actionPL,SIGNAL(triggered()),this,SLOT(interfaceTransit_map()));
}

void MainWindow::showLatestComments( ProtocolBuffer::CommentList* list )
{
    m_ui->list_latestcomment->clear();
    for (int i=0;i<list->comments_size();++i)
    {
        //m_ui->textEdit->append(QString::fromStdString(list->comments(i).content()));
        QListWidgetItem* item = new QListWidgetItem();
        // this need extra caution
        item->setData(Qt::UserRole, qVariantFromValue((void*)&list->comments(i)));
        m_ui->list_latestcomment->addItem(item);
    }
}

void MainWindow::UpdateCurrentLocation( QString s )
{
    m_ui->label_currentlocation->setText(QString::fromLocal8Bit("µ±Ç°Î»ÖÃ£º") + s);
}

void MainWindow::printMessage( const ProtocolBuffer::DMessage* msg )
{
    qDebug()<<"============================ Msg Received ==============================";
    qDebug()<<"From User: "<<msg->fromuser();
    qDebug()<<"To User: "<<msg->touser();
    qDebug()<<"============================ Msg End ===================================";
}

void MainWindow::handleRequestRouting(int uid, const QString& from, const QString& to)
{
    QMessageBox msgbox;
    QString text = QString("%1 is asking your help to guide him.").arg(QString::fromUtf8(getSession()->getUser(uid)->nickname().c_str()));
    QPushButton* googlebutton = msgbox.addButton(QString("google direction"), QMessageBox::AcceptRole);
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setText(text);
    msgbox.setWindowTitle("Routing request");
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(googlebutton);
    int ret = msgbox.exec();
    switch (ret)
    {
    case 0:
        if (msgbox.clickedButton() == googlebutton)
        {
            svc->QueryRoute(from, to, (void*)uid);
        }
        break;
    case QMessageBox::Yes:
        // TODO: Create a simple path from source location to target location
        break;
    case QMessageBox::No:
        // TODO: Send back reject message
        break;
    }
}
