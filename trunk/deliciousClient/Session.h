#ifndef __SESSION__H__INCLUDED__
#define __SESSION__H__INCLUDED__

#include "MapDataSource.h"
#include "qmobilityglobal.h"

#include <string>
#include <QObject>
#include <QMap>
#include <QList>
#include <QBasicTimer>

namespace ProtocolBuffer
{
    class User;
    class UserList;
}

QTM_BEGIN_NAMESPACE
    class QNetworkSession;
QTM_END_NAMESPACE


class Session : public QObject
{
    Q_OBJECT
public:
    Session();
    ~Session();

    void start();

    MapDataSource& getDataSource();
    ProtocolBuffer::User* getUser(); 

    QList<ProtocolBuffer::User*> friends();
    ProtocolBuffer::User* getUser(int uid);

    void UserLocationUpdate(double latitude, double longitude);

public slots:
    void loginMessenger();

signals:
    void ready(bool);

protected:
    void timerEvent(QTimerEvent *);

private:
    void UpdatedUserInfo();
    void FriendsResponse(ProtocolBuffer::UserList*);

    QBasicTimer timer;
    MapDataSource datasource;
    QMap<int, ProtocolBuffer::User*> myfriends;
    ProtocolBuffer::User *user, *infotoupdate;
    google::protobuf::Closure *updatedone;
    QTM_PREPEND_NAMESPACE(QNetworkSession)* network;
    bool info_isdirty;
};

#endif
