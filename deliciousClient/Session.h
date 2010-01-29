#ifndef __SESSION__H__INCLUDED__
#define __SESSION__H__INCLUDED__

#include "MapDataSource.h"
#include "qmobilityglobal.h"

#include <string>
#include <QtCore>

namespace ProtocolBuffer
{
    class User;
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

    void UserLocationUpdate(double latitude, double longitude);

signals:
    void ready(bool);

protected:
    void timerEvent(QTimerEvent *);

private:
    void UpdatedUserInfo();

    QBasicTimer timer;
    MapDataSource datasource;
    ProtocolBuffer::User *user, *infotoupdate;
    google::protobuf::Closure *updatedone;
    QTM_PREPEND_NAMESPACE(QNetworkSession)* network;
    bool info_isdirty;
};

#endif
