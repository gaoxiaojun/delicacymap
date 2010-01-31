#include "StdAfx.h"
#include "Messenger.h"
#include "deliciousDataAdapter.h"
#include "DBResult.h"

#include <boost/bind.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace rclib{
namespace network{

using namespace boost::posix_time;

Messenger::Messenger(boost::asio::io_service& _io)
:ios(_io), msgExpireTimer(_io)
{
    // when we start, we need to get all the messages from db
    msgpool = new MessagesContainer;
    dataadapter = deliciousDataAdapter::GetInstance();
    dataadapter->RetrieveAllNonDeliveredMessages( boost::bind(&Messenger::GetMessageCallback, this, _1));
}

Messenger::~Messenger(void)
{
    msgExpireTimer.cancel();
    delete msgpool;
}

void Messenger::GetMessageCallback( const DBRow& row )
{
    DMessageWrap newmsg;
    // UID 0 is reserved to System Message use
    // but DB constraint enforce that From UID must be a user in DB
    // thus, a FromUID field of NULL indicates it's a system message.

    newmsg.set_fromuser(row["FromUID"].empty() ? 0 : row.GetValueAs<int>("FromUID"));
    newmsg.set_touser(row.GetValueAs<int>("ToUID"));

    // All times stored in DB is in UTC time
    newmsg.AddTime = time_from_string(row["AddTime"]);

    msgpool->insert(newmsg);
}

void Messenger::start()
{
    
}

void Messenger::SendMessageToUser( ProtocolBuffer::DMessage* msg )
{
    // FIXME: Validate msg first!

    tm expiretime = {0};
    expiretime.tm_hour = 1;

    dataadapter->AddMessagesToDB( 
        msg->fromuser(),
        msg->touser(),
        msg->buffer(),
        expiretime);
}

void Messenger::ExpireTimerHandler()
{

}

}
}
