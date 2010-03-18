#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QString>
#include "OfflineMap/ImageCache.h"
#include "OfflineMap/Downloader.h"
#include "MapController.h"

#define maxlisting 40
class ProtobufDataEvent;
class Session;
class GeoBound;
class MapServices;
class MapViewBase;
class MapController;
class QPushButton;

namespace ProtocolBuffer{
    class DMessage;
    class Restaurant;
    class Comment;
    class RestaurantList;
    class CommentList;
    class User;
}
struct showRestaurant;
struct commentAnduser;

namespace Ui {
    class MainWindow;
}


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
    void clearConnections();

public slots:
    void BTHFind();
    void interfaceTransit_map();
    void interfaceTransit_comment();
    void interfaceTransit_favourite();
    //设置成私有成员也可以
    void RestaurantMarkerResponse(const ProtocolBuffer::Restaurant*);
    void showLatestComments( ProtocolBuffer::CommentList* );
    void showUser(const int, ProtocolBuffer::User*);
    void commentCommited(void);
    void commentSuccessed(void);
private slots:
    void printMessage(const ProtocolBuffer::DMessage*);
    void handleRequestRouting(int, const QString&, const QString&);
    
private:
    Ui::MainWindow *m_ui;
    MapViewBase *navi;
    MapServices *svc;
    QPushButton *btn_zoomIn, *btn_zoomOut;
    MapController controller;
    ImageCache imageCache;
    Downloader downloader;
    Session *session;
    showRestaurant * showrestaurant;
};

#endif // MAINWINDOW_H
