#ifndef USERINFOFORM_H
#define USERINFOFORM_H

#include <QWidget>

#include "OfflineMap/GeoCoord.h"

namespace Ui {
    class UserInfoForm;
}

class MapServices;
class Session;

class UserInfoForm : public QWidget {
    Q_OBJECT
public:
    UserInfoForm(QWidget *parent = 0);
    ~UserInfoForm();

    void setLocation(GeoPoint);
    void setUID(int uid);
    void setSession(Session* s) { this->s = s; }
    Session* getSession() { return s; }

protected:
    void changeEvent(QEvent *e);

private:
    void _locationResolved();
private slots:
    void on_btnSub_clicked();
    void locationResolved();
private:
    Ui::UserInfoForm *ui;
    MapServices *svc;
    Session* s;
    int uid, pendingOperations;
    QString streetName;
};

#endif // USERINFOFORM_H
