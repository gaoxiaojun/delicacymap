#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QString>
#include <QGeoSatelliteInfo>
#include "OfflineMap/ImageCache.h"
#include "OfflineMap/Downloader.h"
#include "MapController.h"

class Session;
class GeoBound;
class MapServices;
class MapViewBase;
class MapController;
class QPushButton;
class QTimeLine;
class QMenu;

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
    void handlePanelClosing(PanelWidget*);
    void handleBtnConfirmClicked();
    void handleBtnCancelClicked();
    void handleBtnGPSInfoClicked();
    void handleBtnMapClicked();
    void handleSearchClicked();
    void handleSearchResponse(ProtocolBuffer::SearchResult*);
    void updateGPSInfo_InView(QList<QGeoSatelliteInfo>);
    void updateGPSInfo_Used(QList<QGeoSatelliteInfo>);
    void commentCommited(QList<GeoPoint> *x);
    void commentSuccessed(void);
    void handleRequestRouting(int, const ProtocolBuffer::LocationEx* from, const ProtocolBuffer::LocationEx* to);
    void AddMarkerClicked();
    void showSystemMenu();
    void showSubscriptionTip(const ProtocolBuffer::CommentList*);
    void findCommentByLink(const QString&);

    void sendDialog();
    void dialogwith(int);
    void HandleUserMessage(const ProtocolBuffer::DMessage* );
private:
    void searchResponse(ProtocolBuffer::SearchResult*);
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
};

#endif // MAINWINDOW_H
