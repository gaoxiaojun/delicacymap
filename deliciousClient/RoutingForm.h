#ifndef ROUTINGFORM_H
#define ROUTINGFORM_H

#include <QWidget>

namespace Ui {
    class RoutingForm;
}

class QLineEdit;
class Session;

namespace ProtocolBuffer{
    class User;
}
class RoutingForm : public QWidget {
    Q_OBJECT
public:
    RoutingForm(QWidget *parent = 0);
    void setSession(Session* session) { s = session; }
    Session* getSession() const { return s; }
    void setFriends();
    ~RoutingForm();

signals:
    void doRoutingRequest(QString from, QString to, int uid);

protected:
    void changeEvent(QEvent *e);
    void setLocalPositionIfEmpty(QLineEdit*);

private:
    Ui::RoutingForm *ui;
    Session* s;
private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_switch_clicked();
};

#endif // ROUTINGFORM_H
