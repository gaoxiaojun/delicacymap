#ifndef USER_H
#define USER_H

#include <QtGui/QWidget>
#include "ui_user.h"
namespace ProtocolBuffer{
    class User;
};
class Session;

class usr : public QWidget
{
    Q_OBJECT

public:
    usr(QWidget *parent = 0, Qt::WFlags flags = 0);
    void setSession(Session *);
    Session* getSession();
    void disconnectall();

    ~usr();
public slots:
    void setusr(int _uid,const char * name,const char * mail,const char * addtime);
    void SubscribeToUser();
    void deleteFriend();
    void addFriend();
signals:
    void FriendDelete(int uid);
    void FriendAdd(int uid);
private:
    int uid;
    Ui::UserClass *ui;
    Session * session;
};

#endif // USER_H