#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QString>
#include <QGeoSatelliteInfo>
#include "OfflineMap/ImageCache.h"
#include "OfflineMap/Downloader.h"
#include "MapController.h"
#include "user.h"
class Session;
class GeoBound;
class MapServices;
class MapViewBase;
class MapController;
class QPushButton;
class QTimeLine;
class QMenu;
class RoutingForm;
class QListWidgetItem;

namespace ProtocolBuffer{
    class LocationEx;
    class DMessage;
    class Restaurant;
    class Comment;
    class RestaurantList;
    class CommentList;
    class User;
    class SearchResult;
}

namespace Ui {
    class MainWindow;
}

class RestaurantMarkerItem;
class UserMarkerItem;
class PanelWidget;

class MainWindow : public QMainWindow 
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(Session *s = NULL, QWidget *parent = NULL);
    virtual ~MainWindow();
    void changeSession(Session *);
    Session* getSession();
protected:
    virtual void changeEvent(QEvent *e);

private slots:
    void BTHFind();
    void RestaurantMarkerResponse(RestaurantMarkerItem*);
    void UserMarkerResponse(UserMarkerItem*);
    void handleBtnConfirmClicked();
    void handleBtnCancelClicked();
    void handleBtnGPSInfoClicked();
    void handleBtnMapClicked();
    void handleSearchClicked();
    void handleSearchResponse(ProtocolBuffer::SearchResult*);
    void updateGPSInfo_InView(QList<QGeoSatelliteInfo>);
    void updateGPSInfo_Used(QList<QGeoSatelliteInfo>);
    void handleRequestRouting(int, const ProtocolBuffer::LocationEx* from, const ProtocolBuffer::LocationEx* to);
    void handleRoutingReply(int, QList<GeoPoint>);
    void AddMarkerClicked();
    void showSystemMenu();
    void showSubscriptionTip(const ProtocolBuffer::CommentList*);
    void findCommentByLink(const QString&);
    void locateByCellID();
   
    void drawRoute(QList<GeoPoint>* route);
    void doRoutingRequest(GeoPoint, GeoPoint, int);
    void startRouting(const ProtocolBuffer::Restaurant *);
    void sendDialog();
    void dialogwith(int);
    void transToFriend();
    void HandleUserMessage(const ProtocolBuffer::DMessage* );
    void showFriendsInfo(QListWidgetItem* usr);
    void FriChanged(bool b,int uid);
private:
    void searchResponse(ProtocolBuffer::SearchResult*);
    void locateByCellIDClosure(InaccurateGeoPoint*);
    void createMenu();
    
private:
    QTimeLine *pan_Btn_timeline;
    Ui::MainWindow *m_ui;
    QMenu* mainMenu;
    MapViewBase *navi;
    MapServices *svc;
    QPushButton *btn_zoomIn, *btn_zoomOut;
    MapController controller;
    ImageCache imageCache;
    Downloader downloader;
    Session *session;
    usr  usrForm;
};

#endif // MAINWINDOW_H
