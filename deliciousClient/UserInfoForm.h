#ifndef USERINFOFORM_H
#define USERINFOFORM_H

#include <QWidget>

#include "OfflineMap/GeoCoord.h"

namespace Ui {
    class UserInfoForm;
}

class MapServices;

class UserInfoForm : public QWidget {
    Q_OBJECT
public:
    UserInfoForm(QWidget *parent = 0);
    ~UserInfoForm();

    void setLocation(GeoPoint);

protected:
    void changeEvent(QEvent *e);

private:
    void _locationResolved();
private slots:
    void locationResolved();
private:
    Ui::UserInfoForm *ui;
    MapServices *svc;
    QString streetName;
};

#endif // USERINFOFORM_H
