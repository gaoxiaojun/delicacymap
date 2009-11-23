#pragma once
#include "MapDataSource.h"

#include <string>

namespace ProtocolBuffer
{
    class User;
}


class Session
{
public:
    Session();
    ~Session();

    MapDataSource& getDataSource();
    ProtocolBuffer::User* getUser(); 

private:
    MapDataSource datasource;
    ProtocolBuffer::User *user;
};
