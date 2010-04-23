#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bluetoothmanager.h"
#include "QTProtobufWaitResponse.h"
#include "MapController.h"
#include "Session.h"
#include "MapProtocol.pb.h"
#include "RestaurantInfoForm.h"
#include "SearchResultForm.h"
#include "RoutingForm.h"
#include "UserInfoForm.h"
#include "Configurations.h"
#include "OfflineMap/MarkerItem.h"
#include "OfflineMap/MapViewBase.h"
#include "OfflineMap/MapDecorators.h"
#include "OfflineMap/MapServices.h"
#include "OfflineMap/GeoCoord.h"
#include <QMenu>
#include <QMessageBox>
#include <QStringList>
#include <QDebug>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>

#include <boost/foreach.hpp>

using namespace ProtocolBuffer;
QTM_USE_NAMESPACE

MainWindow::MainWindow(Session *s, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    session(NULL)
{
    m_ui->setupUi(this);
    createMenu();

    navi = new MapViewBase(this);
    navi->setDecorator(new MoveDecorator(navi, Configurations::Instance().SmoothPan()));
    navi->setCache(&imageCache);
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
    imageCache.setCacheDBPath(QString::fromUtf8(Configurations::Instance().MapPath().c_str()));
    btn_zoomIn = new QPushButton(QIcon(":/Icons/zoomin.png"), "", navi);
    btn_zoomIn->setGeometry(8, 8, 64, 64);
    btn_zoomIn->setIconSize(QSize(64, 64));
    btn_zoomIn->setFlat(true);
    btn_zoomOut = new QPushButton(QIcon(":/Icons/zoomout.png"), "", navi);
    btn_zoomOut->setGeometry(80, 8, 64, 64);
    btn_zoomOut->setIconSize(QSize(64, 64));
    btn_zoomOut->setFlat(true);
    controller.setMapView(navi);

    connect(m_ui->btn_menu, SIGNAL(clicked()), SLOT(showSystemMenu()));
    connect(m_ui->btn_addMarker, SIGNAL(clicked()), SLOT(AddMarkerClicked()));
    connect(m_ui->btn_addMarker_confirm, SIGNAL(clicked()), SLOT(handleBtnConfirmClicked()));
    connect(m_ui->btn_addMarker_cancel, SIGNAL(clicked()), SLOT(handleBtnCancelClicked()));
    connect(m_ui->toolButton_GPS, SIGNAL(clicked()), SLOT(handleBtnGPSInfoClicked()));
    connect(m_ui->toolButton_Map, SIGNAL(clicked()), SLOT(handleBtnMapClicked()));
    connect(m_ui->toolButton_Search, SIGNAL(clicked()), SLOT(handleSearchClicked()));
    connect(navi, SIGNAL(canZoomIn(bool)), btn_zoomIn, SLOT(setEnabled(bool)));
    connect(navi, SIGNAL(canZoomOut(bool)), btn_zoomOut, SLOT(setEnabled(bool)));
    connect(btn_zoomIn, SIGNAL(clicked()), navi, SLOT(zoomIn()));
    connect(btn_zoomOut, SIGNAL(clicked()), navi, SLOT(zoomOut()));
    connect(navi, SIGNAL(boundsChange(const GeoBound&)), &controller, SLOT(MapViewBoundsChange(const GeoBound&)));
   
    //ZZQ edited,编辑一个slot,专门用来显示餐厅信息
    connect(navi,SIGNAL(restaurantMarkerClicked(RestaurantMarkerItem*)),SLOT(RestaurantMarkerResponse(RestaurantMarkerItem*)));
    connect(navi, SIGNAL(userMarkerClicked(UserMarkerItem*)), SLOT(UserMarkerResponse(UserMarkerItem*)));
    connect(m_ui->sendButton,SIGNAL(clicked()),this,SLOT(sendDialog()));
    connect(m_ui->FriendlistWidget,SIGNAL(currentRowChanged(int)),this,SLOT(dialogwith(int)));
    connect(m_ui->toolButton_Friends,SIGNAL(clicked()),this,SLOT(transToFriend()));
    connect(&controller, SIGNAL(currentLocationUpdate(InaccurateGeoPoint)), navi, SLOT(setSelfLocation(const InaccurateGeoPoint&)));
    connect(&controller, SIGNAL(subscriptionArrived(const ProtocolBuffer::CommentList*)), this, SLOT(showSubscriptionTip(const ProtocolBuffer::CommentList*)));
    connect(&controller, SIGNAL(SysMsgRequestRouting(int, const ProtocolBuffer::LocationEx*, const ProtocolBuffer::LocationEx*)), this, SLOT(handleRequestRouting(int, const ProtocolBuffer::LocationEx*, const ProtocolBuffer::LocationEx*)));
    connect(&controller, SIGNAL(SysMsgUserLocationUpdate(int, GeoPoint)), navi, SLOT(updateUserLocation(int, const GeoPoint&)));
    QGeoSatelliteInfoSource* src = controller.getSatelliteInfoSource();
    if (src)
    {
        connect(src, SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)), SLOT(updateGPSInfo_InView(QList<QGeoSatelliteInfo>)));
        connect(src, SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)), SLOT(updateGPSInfo_Used(QList<QGeoSatelliteInfo>)));
    }
    if (controller.getPositionInfoSource())
        connect(m_ui->btn_StartGPS, SIGNAL(clicked()), controller.getPositionInfoSource(), SLOT(startUpdates()));

    navi->setZoomLevel(15);
    navi->setGeoCoords(GeoCoord(39.96067508327288), GeoCoord(116.35796070098877));

    qRegisterMetaType<ProtocolBuffer::SearchResult*>("ProtocolBuffer::SearchResult*");

    svc = new MapServices;
//    pan_Btn_timeline = new QTimeLine(300, this);
//    pan_Btn_timeline->setCurveShape(QTimeLine::EaseOutCurve);
//    pan_Btn_timeline->setDirection(QTimeLine::Forward);
    m_ui->btn_addMarker_cancel->hide();
    m_ui->btn_addMarker_confirm->hide();
    changeSession(s);
    //新修改
    this->m_ui->stackedWidget->insertWidget(0, navi);
    this->m_ui->stackedWidget->setCurrentWidget(navi);
    this->m_ui->DialogtextEdit->setDisabled(true);
    int uid;
    QList<ProtocolBuffer::User*> friendlist=this->getSession()->friends();
    for(int i=0;i<friendlist.count();i++)
    {
        uid=friendlist.value(i)->uid();
        m_ui->FriendlistWidget->addItem(friendlist.value(i)->nickname().c_str());
        m_ui->FriendlistWidget->setCurrentRow(i);
        m_ui->FriendlistWidget->currentItem()->setData(Qt::WhatsThisRole,uid);
    }
}

MainWindow::~MainWindow()
{
    delete mainMenu;
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

void MainWindow::createMenu()
{
    mainMenu = new QMenu();
    mainMenu->setStyleSheet("");
    mainMenu->addAction(tr("Locate by cell network"), this, SLOT(locateByCellID()));
    mainMenu->addSeparator();
    mainMenu->addAction(tr("Quit"), this, SLOT(close()));
}

void MainWindow::locateByCellID()
{
    qDebug()<<"Start Locating by Cell.";
    InaccurateGeoPoint *p = new InaccurateGeoPoint;
    svc->LocationByCellID(*p, google::protobuf::NewCallback(this, &MainWindow::locateByCellIDClosure, p));
}

void MainWindow::locateByCellIDClosure(InaccurateGeoPoint *p)
{
    qDebug()<<"Get location: lat="<<p->p.lat.getDouble()<<". lng="<<p->p.lng.getDouble()<<". acc="<<p->accuracy<<".";
    navi->setSelfLocation(*p);
    delete p;
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
//        disconnect(&session->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), this, SLOT(printMessage(const ProtocolBuffer::DMessage*)));
        disconnect(&session->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), &controller, SLOT(HandleSystemMessages(const ProtocolBuffer::DMessage*)));
    }
    session = s;
    if (s)
    {
//        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), this, SLOT(printMessage(const ProtocolBuffer::DMessage*)));
        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)), &controller, SLOT(HandleSystemMessages(const ProtocolBuffer::DMessage*)));
        connect(&s->getDataSource(), SIGNAL(messageReceived(const ProtocolBuffer::DMessage*)),this,SLOT(HandleUserMessage(const ProtocolBuffer::DMessage* )));   
    }
    controller.setSession(s);
}

Session* MainWindow::getSession()
{
    return session;
}

void MainWindow::AddMarkerClicked()
{
    m_ui->btn_addMarker->hide();
    m_ui->btn_addMarker_cancel->show();
    m_ui->btn_addMarker_confirm->show();

    RestaurantMarkerItem* localmarker = new RestaurantMarkerItem();
    navi->addLocalMarker(localmarker);
}

void MainWindow::handleSearchClicked()
{
    QString searchText = m_ui->lineEdit_Search->text();
    if (searchText.size() > 0)
    {
        std::string searchTextUtf8(searchText.toUtf8().constData());
        ProtocolBuffer::SearchResult* result = new ProtocolBuffer::SearchResult;
        google::protobuf::Closure* done = google::protobuf::NewCallback(this, &MainWindow::searchResponse, result);
        getSession()->getDataSource().Search(searchTextUtf8, result, done);
    }
}

void MainWindow::searchResponse(ProtocolBuffer::SearchResult *result)
{
    if (result->has_restaurants() || result->has_users())
    {
        QMetaObject::invokeMethod(this, "handleSearchResponse", Q_ARG(ProtocolBuffer::SearchResult*, result));
    }
}

void MainWindow::handleSearchResponse(ProtocolBuffer::SearchResult *result)
{
    SearchResultForm *form = new SearchResultForm(result, navi);
    form->setPanel(navi->addBlockingPanel(form));
}

void MainWindow::handleBtnMapClicked()
{
    m_ui->stackedWidget->setCurrentWidget(navi);
    if(controller.getSatelliteInfoSource())
        controller.getSatelliteInfoSource()->stopUpdates();
}

void MainWindow::handleBtnGPSInfoClicked()
{
    QGeoSatelliteInfoSource* src = controller.getSatelliteInfoSource();
    connect(src, SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)), SLOT(updateGPSInfo_InView(QList<QGeoSatelliteInfo>)));
    connect(src, SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)), SLOT(updateGPSInfo_Used(QList<QGeoSatelliteInfo>)));
    if(src)
    {
        src->startUpdates();
        m_ui->stackedWidget->setCurrentWidget(m_ui->page_gps);
    }
}

void MainWindow::updateGPSInfo_InView(QList<QGeoSatelliteInfo> satellites)
{
    m_ui->label_Sat_In_View->setText( QString::number(satellites.size()) );
}

void MainWindow::updateGPSInfo_Used(QList<QGeoSatelliteInfo> satellites)
{
    m_ui->label_Sat_Used->setText( QString::number(satellites.size()) );
}

void MainWindow::handleRequestRouting(int uid, const ProtocolBuffer::LocationEx* from, const ProtocolBuffer::LocationEx* to)
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
            if (from->has_location_st())
            {
                svc->QueryRoute(
                        QString::fromUtf8(from->location_st().c_str(), from->location_st().length()),
                        QString::fromUtf8(to->location_st().c_str(), to->location_st().length()),
                        *result,
                        closure);
            }
            else
            {
                svc->QueryRoute(
                        GeoPoint(from->location_geo().latitude(), from->location_geo().longitude()),
                        GeoPoint(to->location_geo().latitude(), to->location_geo().longitude()),
                        *result,
                        closure);
            }
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
        connect(form,SIGNAL(RequestByRestaurant(const ProtocolBuffer::Restaurant*)),
            this,SLOT(startRouting(const ProtocolBuffer::Restaurant *)));
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

void MainWindow::UserMarkerResponse(UserMarkerItem *userMarker)
{
    UserInfoForm *item = new UserInfoForm();
    item->setLocation(userMarker->getPos());
    navi->addBlockingPanel(item, userMarker);
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
    m_ui->btn_addMarker->show();
    m_ui->btn_addMarker_confirm->hide();
    m_ui->btn_addMarker_cancel->hide();
}

void MainWindow::sendDialog()
{
    //得到消息框   
    QString message=QString(m_ui->messageLineEdit->text());
    //得到当前聊天的用户
    QString tousrName=m_ui->FriendlistWidget->currentItem()->text();
    QString fromusrName=QString(this->getSession()->getUser()->nickname().c_str());
    int touid=m_ui->FriendlistWidget->currentItem()->data(Qt::WhatsThisRole).toInt();
    int fromuid=this->getSession()->getUser()->uid();
    //发送消息
    this->getSession()->getDataSource().SendMessage(fromuid,touid,string(message.toUtf8().constData()));
    //消息框处理,
    QString old=m_ui->DialogtextEdit->toPlainText();
    old=old+fromusrName+" says:  "+message+"\n";
    this->m_ui->FriendlistWidget->currentItem()->setData(Qt::AccessibleTextRole,old);
    this->m_ui->DialogtextEdit->setText(old);
    this->m_ui->messageLineEdit->clear();
}

void MainWindow::dialogwith(const int current)
{
    m_ui->chatLabel->setText(QString("chatting with %1").arg(m_ui->FriendlistWidget->currentItem()->text()));
    //改变聊天对象时，应该保存当前的聊天记录，并显示其他人的聊天记录...
    m_ui->DialogtextEdit->clear();
    m_ui->DialogtextEdit->setText(m_ui->FriendlistWidget->currentItem()->data(Qt::AccessibleTextRole).toString());
    
}

void MainWindow::HandleUserMessage(const ProtocolBuffer::DMessage* m)
{
    //是用户信息
    if(!m->issystemmessage())
    {
        this->m_ui->stackedWidget->setCurrentIndex(1);
        int fromuid=m->fromuser();
        int touid=m->touser();
        if(touid!=this->getSession()->getUser()->uid())
        {
            qDebug()<<"Why I can receive the message which not belong to me?";
            exit(0);//压根不是发往该用户的，则直接结束
        }
        for(int i=0;i<m_ui->FriendlistWidget->count();i++)
        {
            //如果当前的Item就是发送人
            if(m_ui->FriendlistWidget->item(i)->data(Qt::WhatsThisRole).toInt()==fromuid)
            {
                QString fromname=this->m_ui->FriendlistWidget->item(i)->text();
                m_ui->FriendlistWidget->setCurrentRow(i);
                QString old=m_ui->FriendlistWidget->currentItem()->data(Qt::AccessibleTextRole).toString();
                old=old+fromname+" say : "+QString::fromUtf8(m->text().c_str())+'\n';
                this->m_ui->DialogtextEdit->setText(old);
                this->m_ui->FriendlistWidget->currentItem()->setData(Qt::AccessibleTextRole,old);
            
            }
        }
    }
}

void MainWindow::showSystemMenu()
{
    mainMenu->popup(this->mapToGlobal(m_ui->btn_menu->geometry().topLeft() - QPoint(0, mainMenu->sizeHint().height() + 3)));
}

void MainWindow::showSubscriptionTip(const ProtocolBuffer::CommentList *list)
{
    static const QString commentTemplate = tr("<a href='user_click#%3'>%1</a> has posted a new comment on <a href='restaurant_click#%4'>%2</a>.");
    static const QString commentTemplateOnlyUser = tr("<a href='user_click#%2'>%1</a> has posted a new comment.");
    static const QString commentTemplateOnlyRestaurant = tr("<a href='restaurant_click#%2'>%1</a> has a new comment.");
    QStringList labeltext;

    for (int i=0;i<list->comments_size();++i)
    {
        const ProtocolBuffer::Comment& c = list->comments(i);
        if (c.has_restaurantinfo())
        {
            navi->addRestaurantMarker(&c.restaurantinfo());
        }
        QString text;
        if (c.has_userinfo() && c.has_restaurantinfo())
            text = commentTemplate.arg(QString::fromUtf8(c.userinfo().nickname().c_str()), QString::fromUtf8(c.restaurantinfo().name().c_str())).arg(c.uid()).arg(c.rid());
        else if (c.has_userinfo())
            text = commentTemplateOnlyUser.arg(QString::fromUtf8(c.userinfo().nickname().c_str())).arg(c.uid());
        else if (c.has_restaurantinfo())
            text = commentTemplateOnlyRestaurant.arg(QString::fromUtf8(c.restaurantinfo().name().c_str())).arg(c.rid());

        if (!text.isEmpty())
        {
            labeltext<<text;
        }
    }
    if (!labeltext.empty())
    {
        QLabel *label = new QLabel(labeltext.join("\n"));
        label->setWordWrap(true);
        label->setMaximumWidth(navi->width()/3);
        connect(label, SIGNAL(linkActivated(QString)), this, SLOT(findCommentByLink(QString)));
        navi->showTip(label);
    }
    delete list;
}

void MainWindow::findCommentByLink(const QString &link)
{
    QLabel* source = static_cast<QLabel*>(sender());
    Q_ASSERT(source);
    if (link.startsWith("restaurant_click#"))
    {
        int rid = link.mid(link.indexOf('#') + 1).toInt();
        navi->centerOn(navi->getRestaurantMarker(rid)); //guaranteed to exist.
    }
    else if (link.startsWith("user_click#"))
    {
        int uid = link.mid(link.indexOf('#') + 1).toInt();
    }
}

void MainWindow::transToFriend()
{
    m_ui->stackedWidget->setCurrentIndex(2);
    m_ui->FriendlistWidget->setCurrentRow(0);
}

void MainWindow::startRouting(const ProtocolBuffer::Restaurant *res)
{
    RoutingForm * r_ui=new RoutingForm;
    //navi->addBlockingPanel(r_ui);
    r_ui->setService(svc);
    r_ui->setSession(this->getSession());
    r_ui->setFriends();
    r_ui->setToLocation(QString::fromUtf8(res->name().c_str()));
    r_ui->show();
    connect(r_ui,SIGNAL(doRoutingRequest(GeoPoint,GeoPoint,int)),this,SLOT(doRoutingRequest(GeoPoint,GeoPoint,int)));
}
void MainWindow::doRoutingRequest(GeoPoint from, GeoPoint to, int uid)
{
    if (!from.isValid() && controller.getPositionInfoSource() && controller.getPositionInfoSource()->lastKnownPosition(false).isValid())
    {
        from.lat.setDouble(controller.getPositionInfoSource()->lastKnownPosition(false).coordinate().latitude());
        from.lng.setDouble(controller.getPositionInfoSource()->lastKnownPosition(false).coordinate().longitude());
    }
    if (!to.isValid() && controller.getPositionInfoSource() && controller.getPositionInfoSource()->lastKnownPosition(false).isValid())
    {
        to.lat.setDouble(controller.getPositionInfoSource()->lastKnownPosition(false).coordinate().latitude());
        to.lng.setDouble(controller.getPositionInfoSource()->lastKnownPosition(false).coordinate().longitude());
    }
    if (from.isValid() && to.isValid() && from != to)
    {
        if(uid==-1)
        {
            QList<GeoPoint> *route=new QList<GeoPoint>;
            google::protobuf::Closure * done=google::protobuf::NewCallback(this,&MainWindow::drawRoute,route);
            this->svc->QueryRoute(from,to,*route,done);//google direction
        }
        else
        {
            this->getSession()->SendRoutingRequest(from,to,uid);
        }
    }
}
void MainWindow::drawRoute(QList<GeoPoint>* route)
{
    this->navi->addRoute(*route);
    delete route;
}
