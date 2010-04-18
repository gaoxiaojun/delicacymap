#ifndef ROUTINGFORM_H
#define ROUTINGFORM_H

#include <QWidget>
#include "OfflineMap/GeoCoord.h"

namespace Ui {
    class RoutingForm;
}

class QLineEdit;
class Session;
class MapServices;

namespace ProtocolBuffer{
    class User;
}
class RoutingForm : public QWidget {
    Q_OBJECT
public:
    RoutingForm(QWidget *parent = 0);
    ~RoutingForm();
    void setService(MapServices* svc) { this->svc = svc; }
    MapServices* getService() const { return svc; }
    void setSession(Session* session) { s = session; }
    Session* getSession() const { return s; }
    void setFriends();
    void setFromLocation(const QString&);
    void setToLocation(const QString&);

signals:
    void doRoutingRequest(GeoPoint from, GeoPoint to, int uid);

protected:
    void changeEvent(QEvent *e);
    void setLocalPositionIfEmpty(QLineEdit*);

private:
    Ui::RoutingForm *ui;
    Session* s;
    MapServices *svc;
    GeoPoint from, to;
private slots:
    void on_lineEdit_to_editingFinished();
    void on_lineEdit_from_editingFinished();
    void on_pushButton_confirm_clicked();
    void on_pushButton_switch_clicked();
};

#endif // ROUTINGFORM_H
