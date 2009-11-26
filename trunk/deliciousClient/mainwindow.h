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
    Session* getSession();

protected:
    virtual void changeEvent(QEvent *e);
    void clearConnections();

public slots:
    void BTHFind();
    void interfaceTransit_map();
    void interfaceTransit_comment();
    void interfaceTransit_favourite();

    void showLatestCommentsFor( int rid );

private:
    Ui::MainWindow *m_ui;
    mapview *navi;
    Session *session;
};

#endif // MAINWINDOW_H
