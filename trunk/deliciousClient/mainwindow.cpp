#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bluetoothmanager.h"
#include "QTProtobufWaitResponse.h"
#include "MapController.h"
#include "Session.h"
#include "MapProtocol.pb.h"
#include "RestaurantInfoForm.h"
#include "OfflineMap/MarkerItem.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MapDecorators.h"
#include "OfflineMap/MapServices.h"
#include "OfflineMap/GeoCoord.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QDebug>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

#include <boost/foreach.hpp>

using namespace ProtocolBuffer;
QTM_USE_NAMESPACE

struct commentAnduser
{
    ProtocolBuffer::Comment comment;
    ProtocolBuffer::User user;
};
struct showRestaurant
{
    ProtocolBuffer::Restaurant restaurant;
    int n;
    commentAnduser commentanduser[maxlisting];
};

MainWindow::MainWindow(Session *s, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    session(NULL)
{
    m_ui->setupUi(this);

    navi = new MapViewBase;
    navi->setDecorator(new MoveDecorator(navi, true));
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
#if _WIN32_WCE
    imageCache.setCacheDBPath("/Storage Card/tiles.map");
#else
    imageCache.setCacheDBPath("tiles.map");
#endif
    navi->setCache(&imageCache);
    btn_zoomIn = new QPushButton(QIcon(":/Icons/zoomin.png"), "", navi);
    btn_zoomIn->setGeometry(8, 8, 64, 64);
    btn_zoomIn->setIconSize(QSize(64, 64));
    btn_zoomIn->setFlat(true);
    btn_zoomOut = new QPushButton(QIcon(":/Icons/zoomout.png"), "", navi);
    btn_zoomOut->setGeometry(80, 8, 64, 64);
    btn_zoomOut->setIconSize(QSize(64, 64));
    btn_zoomOut->setFlat(true);
    connect(m_ui->btn_quit, SIGNAL(clicked()), SLOT(close()));
    connect(m_ui->btn_addMarker, SIGNAL(clicked()), SLOT(AddMarkerClicked()));
    connect(m_ui->btn_addMarker_confirm, SIGNAL(clicked()), SLOT(handleBtnConfirmClicked()));
    connect(m_ui->btn_addMarker_cancel, SIGNAL(clicked()), SLOT(handleBtnCancelClicked()));
    connect(navi, SIGNAL(canZoomIn(bool)), btn_zoomIn, SLOT(setEnabled(bool)));
    connect(navi, SIGNAL(canZoomOut(bool)), btn_zoomOut, SLOT(setEnabled(bool)));
    connect(btn_zoomIn, SIGNAL(clicked()), navi, SLOT(zoomIn()));
    connect(btn_zoomOut, SIGNAL(clicked()), navi, SLOT(zoomOut()));

    controller.setMapView(navi);
    controller.setLocationSource(QGeoPositionInfoSource::createDefaultSource(this));
    connect(navi, SIGNAL(boundsChange(const GeoBound&)), &controller, SLOT(MapViewBoundsChange(const GeoBound&)));
    //ZZQ edited,编辑一个slot,专门用来显示餐厅信息
    connect(navi,SIGNAL(restaurantMarkerClicked(RestaurantMarkerItem*)),SLOT(RestaurantMarkerResponse(RestaurantMarkerItem*)));

    connect(&controller, SIGNAL(currentLocationUpdate(GeoPoint)), navi, SLOT(setSelfLocation(const GeoPoint&)));
    connect(&controller, SIGNAL(SysMsgRequestRouting(int, QString, QString)), this, SLOT(handleRequestRouting(int, const QString&, const QString&)));
    connect(&controller, SIGNAL(SysMsgUserLocationUpdate(int, GeoPoint)), navi, SLOT(updateUserLocation(int, const GeoPoint&)));

    navi->setZoomLevel(15);
    navi->setGeoCoords(GeoCoord(39.96067508327288), GeoCoord(116.35796070098877));

    svc = new MapServices;
//    pan_Btn_timeline = new QTimeLine(300, this);
//    pan_Btn_timeline->setCurveShape(QTimeLine::EaseOutCurve);
//    pan_Btn_timeline->setDirection(QTimeLine::Forward);
    m_ui->btn_addMarker_cancel->hide();
    m_ui->btn_addMarker_confirm->hide();

    int index = this->m_ui->stackedWidget->insertWidget(0,navi);
    qDebug()<<index<<endl;
    this->m_ui->stackedWidget->setCurrentWidget(navi);
    changeSession(s);
}

MainWindow::~MainWindow()
{
    delete navi;
    delete m_ui;
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
        disconnect(&session->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), &controller, SLOT(HandleSystemMessages(const ProtocolBuffer::DMessage*)));
    }
    session = s;
    if (s)
    {
        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), this, SLOT(printMessage(const ProtocolBuffer::DMessage*)));
        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), &controller, SLOT(HandleSystemMessages(const ProtocolBuffer::DMessage*)));
    }
    controller.setSession(s);
}

Session* MainWindow::getSession()
{
    return session;
}

void MainWindow::AddMarkerClicked()
{
    m_ui->btn_quit->hide();
    m_ui->btn_addMarker->hide();
    m_ui->btn_addMarker_cancel->show();
    m_ui->btn_addMarker_confirm->show();

    RestaurantMarkerItem* localmarker = new RestaurantMarkerItem();
    navi->addLocalMarker(localmarker);
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
    QString text = tr("%1 is asking your help to guide him.").arg(QString::fromUtf8(getSession()->getUser(uid)->nickname().c_str()));
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
            QList<GeoPoint> *result = new QList<GeoPoint>;
            google::protobuf::Closure* closure = google::protobuf::NewCallback(&controller, &MapController::AddEditingRouteInFavorOf, (const QList<GeoPoint>*)result, uid);
            svc->QueryRoute(from, to, *result, closure);
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

void MainWindow::RestaurantMarkerResponse(RestaurantMarkerItem* res)
{
    if (!res->isFakeMarker())
    {
        RestaurantInfoForm* form = new RestaurantInfoForm();
        form->setRestaurant(res->restaurantInfo());
        form->setSession(getSession());
        navi->addBlockingPanel(form, res);
    }
    else
    {
        QLineEdit *edit = new QLineEdit;
        if (res->restaurantInfo())
            edit->setText(QString::fromUtf8( res->restaurantInfo()->name().c_str() ));
        PanelWidget* panel = navi->addBlockingPanel(edit, res);
        connect(panel, SIGNAL(closing(PanelWidget*)), SLOT(handlePanelClosing(PanelWidget*)), Qt::DirectConnection);
    }
}

void MainWindow::handlePanelClosing(PanelWidget *w)
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(w->widget());
    if (edit && edit->text().size() > 0 && w->tiedTo() && w->tiedTo()->type() == RestaurantMarkerItem::Type)
    {
        RestaurantMarkerItem* item = static_cast<RestaurantMarkerItem*>(w->tiedTo());
        if (item->isFakeMarker())
        {
            std::string name(edit->text().toUtf8().constData());
            item->mutableRestaurantInfo()->set_name(name);
        }
    }
}

void MainWindow::handleBtnConfirmClicked()
{
    QList<ZoomSensitiveItem*> locals = navi->localMarkers();
    BOOST_FOREACH(ZoomSensitiveItem* it, locals)
    {
        switch (it->type())
        {
        case RestaurantMarkerItem::Type:
            {
                RestaurantMarkerItem* r = static_cast<RestaurantMarkerItem*>(it);
                Q_ASSERT( r->isFakeMarker() );
                ProtocolBuffer::Restaurant* rinfo = r->mutableRestaurantInfo(); // small optimization, save one allocation
                int sendRequestToServer = false;
                if (rinfo->name().empty())
                {
                    navi->ensureVisible(it);
                    QMessageBox msgbox;
                    msgbox.setIcon(QMessageBox::Question);
                    msgbox.setText(tr("Do you want to continue? Coninueing will discard this marker."));
                    msgbox.setWindowTitle(tr("Restautant without a name"));
                    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgbox.setDefaultButton(QMessageBox::No);
                    int ret = msgbox.exec();
                    if (ret == QMessageBox::Yes)
                        navi->removeItem(it);
                    continue;
                }
                google::protobuf::Closure* promote = google::protobuf::NewCallback(
                        r,
                        &RestaurantMarkerItem::PromoteToRealMarker,
                        const_cast<const ProtocolBuffer::Restaurant*>(rinfo));
                getSession()->getDataSource().AddRestaurant(rinfo->name(), rinfo->location().latitude(), rinfo->location().longitude(), rinfo, promote);
            }
            break;
        default:
            qDebug()<<"Unhandled local marker type:"<<it->type();
        }
        navi->removeFromLocal(it);
    }
    if (!navi->hasLocalMarker())
    {
        m_ui->btn_quit->show();
        m_ui->btn_addMarker->show();
        m_ui->btn_addMarker_confirm->hide();
        m_ui->btn_addMarker_cancel->hide();
    }
}

void MainWindow::handleBtnCancelClicked()
{
    if (navi->hasLocalMarker())
    {
        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Question);
        msgbox.setText(tr("Do you want to discard all previous work?"));
        msgbox.setWindowTitle(tr("Unsaved local markers."));
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgbox.setDefaultButton(QMessageBox::No);
        int ret = msgbox.exec();
        switch (ret)
        {
        case QMessageBox::Yes:
            {
                QList<ZoomSensitiveItem*> locals = navi->localMarkers();
                BOOST_FOREACH(ZoomSensitiveItem* it, locals)
                {
                    navi->removeItem(it);
                }
            }
            break;
        case QMessageBox::No:
            return;
        }
    }
    m_ui->btn_quit->show();
    m_ui->btn_addMarker->show();
    m_ui->btn_addMarker_confirm->hide();
    m_ui->btn_addMarker_cancel->hide();
}

void MainWindow::commentSuccessed(void)
{
    qDebug()<<"Successed!!!";

//     QMessageBox msgbox;
//     msgbox.setIcon(QMessageBox::Information);
//     msgbox.setText("Comment has been added!");
//     msgbox.setWindowTitle("OK");
//     msgbox.setStandardButtons(QMessageBox::Yes);
//     msgbox.setDefaultButton(QMessageBox::Yes);
//     msgbox.exec();
    
}

void MainWindow::commentCommited(void)
{
        //先取得当前的用户名,然后再提交
    QString content=this->m_ui->textComment->toPlainText();
    std::string usrname=this->getSession()->getUser()->nickname();
    if (!content.isEmpty())
    {   
        QString str=QString("%1:   %2").arg(usrname.c_str()).arg(content);
        m_ui->list_latestcomment->addItem(new QListWidgetItem(str));
        m_ui->textComment->setText("");
        //提交给服务器
        ProtocolBuffer::Comment *newComment=new ProtocolBuffer::Comment();

        google::protobuf::Closure* commentadded;
        QByteArray utf8Content = content.toUtf8();
        std::string contentStr(utf8Content.constData(), utf8Content.size());
        commentadded=google::protobuf::NewCallback(this,&MainWindow::commentSuccessed);
        this->getSession()->getDataSource().AddCommentForRestaurant(
            this->showrestaurant->restaurant.rid(),
            this->getSession()->getUser()->uid(),
            contentStr,newComment,commentadded);
    }
    else
    {
        QMessageBox msgbox;
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText("Comment Cannot Be Empty!");
        msgbox.setWindowTitle("ERROR");
        msgbox.setStandardButtons(QMessageBox::Yes);
        msgbox.setDefaultButton(QMessageBox::Yes);
        msgbox.exec();
    }

}
  
