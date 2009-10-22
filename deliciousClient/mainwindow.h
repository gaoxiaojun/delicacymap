#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include "mapview.h"
#include "MapDataSource.h"

class ProtobufDataEvent;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    virtual void changeEvent(QEvent *e);
	virtual void customEvent(QEvent *e);
	void clearConnections();

	void postEvent(ProtobufDataEvent*);

private slots:
    void BTHFind();
	void TestRPC();
	void interfaceTransit_map();
	void interfaceTransit_comment();
	void interfaceTransit_favourite();

private:
    Ui::MainWindow *m_ui;
	mapview *navi;
	MapDataSource connman;
};

#endif // MAINWINDOW_H
