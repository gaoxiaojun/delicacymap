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
class QTimeLine;
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
    void UIAnimation_ShowComments(bool show);
    void UIAnimation_ShowAdd(bool show);

private:
    void commentsResponse(ProtocolBuffer::CommentList*);

    Ui::RestaurantInfoForm *ui;
    const ProtocolBuffer::Restaurant* res;
    QTimeLine *timeline;
    QMovie *loading;
    Session *s;
    bool commentsShown, addShown;

private slots:
    void on_btnCommit_clicked();
    void handleCommentList(ProtocolBuffer::CommentList*);
    void frameChange(int);
    void on_btnAdd_clicked();
    void on_btnShow_clicked();
    void on_btnClose_clicked();
};

#endif // RESTAURANTINFOFORM_H
