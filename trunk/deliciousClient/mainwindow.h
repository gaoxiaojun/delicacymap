#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "mapview.h"

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
	void clearConnections();
private slots:
    void BTHFind();
	void interfaceTransit_map();
	void interfaceTransit_comment();
	void interfaceTransit_favourite();

private:
    Ui::MainWindow *m_ui;
	mapview *navi;
};

#endif // MAINWINDOW_H
