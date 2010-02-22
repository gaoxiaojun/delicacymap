#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QString>
#include "OfflineMap/ImageCache.h"
#include "OfflineMap/Downloader.h"

class ProtobufDataEvent;
class Session;
class GeoBound;
class MapServices;
class MapViewBase;
class MapController;

namespace ProtocolBuffer{
    class Restaurant;
    class RestaurantList;
    class CommentList;
    class User;
    class DMessage;
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
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

    void showLatestComments( ProtocolBuffer::CommentList* );

private slots:
    void UpdateCurrentLocation(QString);
    void printMessage(const ProtocolBuffer::DMessage*);
    void GeoCodeHandle(const QString, double, double);
    void ReverseGeoCodeHandle(const QString originalQuery, const QString address);
    void BoundsUpdates(const GeoBound&);
    void handleRequestRouting(int);

private:
    Ui::MainWindow *m_ui;
    MapViewBase *navi;
    MapServices *svc;
    MapController *controller;
    ImageCache imageCache;
    Downloader downloader;
    Session *session;
};

#endif // MAINWINDOW_H
