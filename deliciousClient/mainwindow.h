#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void connectmenuevent();

protected:
    virtual void changeEvent(QEvent *e);
private slots:
    void BTHFind();

private:
    Ui::MainWindow *m_ui;
};

#endif // MAINWINDOW_H
