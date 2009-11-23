#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include "mapview.h"

class ProtobufDataEvent;
class Session;

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

protected:
    virtual void changeEvent(QEvent *e);
    virtual void customEvent(QEvent *e);
    void clearConnections();

    void addRestrauntsToMap(const ProtocolBuffer::RestaurantList &rlist);

    void postEvent(ProtobufDataEvent*);

private slots:
    void BTHFind();
    void interfaceTransit_map();
    void interfaceTransit_comment();
    void interfaceTransit_favourite();

private:
    Ui::MainWindow *m_ui;
    mapview *navi;
    Session *session;
};

#endif // MAINWINDOW_H
