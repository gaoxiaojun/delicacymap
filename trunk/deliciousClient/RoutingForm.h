#ifndef ROUTINGFORM_H
#define ROUTINGFORM_H

#include <QWidget>

namespace Ui {
    class RoutingForm;
}

class QLineEdit;

class RoutingForm : public QWidget {
    Q_OBJECT
public:
    RoutingForm(QWidget *parent = 0);
    ~RoutingForm();

signals:
    void doRoutingRequest(QString from, QString to);

protected:
    void changeEvent(QEvent *e);
    void setLocalPositionIfEmpty(QLineEdit*);

private:
    Ui::RoutingForm *ui;

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_switch_clicked();
};

#endif // ROUTINGFORM_H
