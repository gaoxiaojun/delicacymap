#ifndef RESTAURANTINFOFORM_H
#define RESTAURANTINFOFORM_H

#include <QWidget>
#include <QLabel>

namespace Ui {
    class RestaurantInfoForm;
}

namespace ProtocolBuffer{
    class Restaurant;
    class CommentList;
}

class QMovie;
class Session;

class RestaurantInfoForm : public QWidget {
    Q_OBJECT
public:
    RestaurantInfoForm(QWidget *parent = 0);
    ~RestaurantInfoForm();

    void setSession(Session* session) { s = session; }
    Session* getSession() const { return s; }
    void setRestaurant(const ProtocolBuffer::Restaurant*);

protected:
    void changeEvent(QEvent *e);
private slots:
    void handleCommentList(ProtocolBuffer::CommentList*);
private:
    void commentsResponse(ProtocolBuffer::CommentList*);

    Ui::RestaurantInfoForm *ui;
    const ProtocolBuffer::Restaurant* res;
    QMovie *loading;
    Session *s;

private slots:
    void on_btnAdd_clicked();
    void on_btnShow_clicked();
    void on_btnClose_clicked();
};

#endif // RESTAURANTINFOFORM_H
