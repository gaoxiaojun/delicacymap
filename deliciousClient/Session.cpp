#include "Session.h"
#include "../protocol-buffer-src/MapProtocol.pb.h"

Session::Session()
{
    user = new ProtocolBuffer::User();
}

Session::~Session()
{
    delete user;
}

MapDataSource& Session::getDataSource()
{
    return datasource;
}

ProtocolBuffer::User* Session::getUser()
{
    return user;
}
