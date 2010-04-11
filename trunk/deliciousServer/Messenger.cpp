#include "stdafx.h"
#include "Messenger.h"
#include "deliciousDataAdapter.h"
#include "ProtubufDBRowConversion.h"
#include "DBResult.h"

#include <algorithm>
#include <exception>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>


namespace rclib{
namespace network{

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace boost::multi_index;
using namespace boost::interprocess;

Messenger* Messenger::__messenger = NULL;

Messenger::Messenger(boost::asio::io_service& _io)
:ios(_io), msgExpireTimer(_io)
{
    // when we start, we need to get all the messages from db
    dataadapter = deliciousDataAdapter::GetInstance();
    dataadapter->RetrieveAllNonDeliveredMessages( boost::bind(&Messenger::GetMessageCallback, this, _1));
}

void Messenger::Initialize( boost::asio::io_service &io )
{
    __messenger = new Messenger(io);
}

Messenger* Messenger::GetInstance()
{
    return __messenger;
}

Messenger::~Messenger(void)
{
    msgExpireTimer.cancel();
    BOOST_FOREACH(const DMessageWrap* m, msgpool)
    {
        delete m;
    }
}

void Messenger::GetMessageCallback( const DBRow& row )
{
    DMessageWrap *newmsg = new DMessageWrap;

    newmsg->set_fromuser(row.GetValueAs<int>("FromUID"));
    newmsg->set_touser(row.GetValueAs<int>("ToUID"));
    newmsg->set_msgid(row.GetValueAs<unsigned int>("MSGID"));
    int msgType = row.GetValueAs<int>("MessageType");
    if (msgType)
    {
        newmsg->set_issystemmessage( true );
        newmsg->set_systemmessagetype( (ProtocolBuffer::SystemMessageType)msgType );
    }
    else
        newmsg->set_issystemmessage( false );

    if (newmsg->issystemmessage())
        newmsg->set_buffer(row["MSG"]);
    else
        newmsg->set_text(row["MSG"]);

    // All times stored in DB is in UTC time
    newmsg->AddTime = time_from_string(row["AddTime"]);
    newmsg->ExpireTime = time_from_string(row["ExpireTime"]);

    // This is only used when we initialize, so no locking is needed.
    msgpool.insert(newmsg);
}

void Messenger::start()
{
    typedef MessagesContainer::index<message_ordered_expiretime_tag>::type order_by_expire;
    order_by_expire& view = msgpool.get<message_ordered_expiretime_tag>();

    // The user may actually send message first then start.
    sharable_lock<MutexType> lock(mutex);
    msgExpireTimer.expires_from_now( view.empty() ? minutes(3) : (*view.begin())->ExpireTime - second_clock::universal_time() + seconds(30) );
    msgExpireTimer.async_wait(bind(&Messenger::ExpireTimerHandler, this, placeholders::error));
}

bool Messenger::ProcessSystemMessage( ProtocolBuffer::DMessage* msg )
{
    assert ( msg->issystemmessage() );
    bool handled = false;
    switch (msg->systemmessagetype())
    {
    case ProtocolBuffer::ShareLocationWith:
        {
            // send the user updated location to the appointed user
            usersSharingLocation[msg->fromuser()].insert(msg->touser());
            handled = true;
            break;
        }
    case ProtocolBuffer::StopShareLocationWith:
        {
            usersSharingLocation[msg->fromuser()].erase(msg->touser());
            handled = true;
            break;
        }
    case ProtocolBuffer::UserLocationUpdate:
        if (msg->touser() == 0)
        {
            msg->clear_buffer();
            msg->clear_text();
            BOOST_FOREACH(int usr, usersSharingLocation[msg->fromuser()])
            {
                msg->set_touser(usr);
                ProcessMessage(msg);
            }
            handled = true;
        }
        break;
    case ProtocolBuffer::RequestSubscriptionUpdate:
        {
            // retrieve new comments since last update
            DBResultWrap result = dataadapter->GetSubscriptionForUserSinceLastUpdate( msg->fromuser() );
            ProtocolBuffer::DMessage data;
            ProtocolBuffer::CommentList comments;
            if (!result.empty())
            {
                for (size_t i=0;i<result.getResult()->RowsCount();i++)
                {
                    ProtocolBuffer::Comment* c = comments.add_comments();
                    ProtubufDBRowConversion::Convert(result.getResult()->GetRow(i), *c);
                    ProtubufDBRowConversion::Convert(result.getResult()->GetRow(i), *c->mutable_userinfo());
                }
            }

            data.set_fromuser(0);
            data.set_touser(msg->fromuser());
            data.set_issystemmessage(true);
            data.set_msgid(-1); // this will be set in the call to ProcessMessage
            data.set_systemmessagetype(ProtocolBuffer::SubscriptionData);
            data.set_buffer(comments.SerializeAsString());
            assert( data.IsInitialized() );
            ProcessMessage(&data);

            handled = true;
            break;
        }
    case ProtocolBuffer::SubscribTo:
    case ProtocolBuffer::UnSubscribeFrom:
        {
            if (msg->touser() != 0) // other wise this is (un)subscribing to a restaurant
            {
                dataadapter->ChangeSubsciptionStatusWithUser(
                    msg->fromuser(), 
                    msg->touser(), 
                    msg->systemmessagetype() == ProtocolBuffer::SubscribTo ? true : false);
            }
            else
            {
                ProtocolBuffer::Query query;
                if (msg->has_buffer())
                {
                    query.ParseFromString(msg->buffer());
                    if (query.has_rid())
                    {
                        dataadapter->ChangeSubsciptionStatusWithRestaurant(
                            msg->fromuser(), 
                            query.rid(),
                            msg->systemmessagetype() == ProtocolBuffer::SubscribTo ? true : false);
                    }
                }
            }
            handled = true;
            break;
        }
    }
    return handled;
}

void Messenger::ProcessMessage( ProtocolBuffer::DMessage* msg )
{
    // FIXME: Validate msg first!

    try
    {
        tm expiretime = {0};
        expiretime.tm_min = msg->issystemmessage() && msg->systemmessagetype() == ProtocolBuffer::UserLocationUpdate ? 1 : 5; // Don't propagate location update when we restart.

        size_t msgid = dataadapter->AddMessagesToDB( 
            msg->fromuser(),
            msg->touser(),
            msg->issystemmessage() ? msg->systemmessagetype() : 0,
            msg->issystemmessage() ? msg->buffer() : msg->text(),
            expiretime);
        msg->set_msgid(msgid);

        if (!msg->issystemmessage() || !ProcessSystemMessage(msg))
        {
            DMessageWrap *newmsg = new DMessageWrap;
            newmsg->CopyFrom(*msg);
            newmsg->AddTime = second_clock::universal_time();
            newmsg->ExpireTime = second_clock::universal_time() + time_duration(expiretime.tm_hour, expiretime.tm_min, expiretime.tm_sec);

            scoped_lock<MutexType> lock(mutex);
            msgpool.insert(newmsg);
        }
    }
    catch (const std::exception& e)
    {
        pantheios::log_WARNING("Error adding the msg to internal storage. message discarded. Error Msg:", e.what());
    }
}

void Messenger::ExpireTimerHandler(const boost::system::error_code& err)
{
    if (err)
    {
        pantheios::log_ERROR("Messanger timer error, messages will be cleaned up no more. Error message: [", pantheios::integer(err.value()), "] ", err.message());
    }
    else
    {
        typedef MessagesContainer::index<message_ordered_expiretime_tag>::type order_by_expire;
        order_by_expire& view = msgpool.get<message_ordered_expiretime_tag>();

        scoped_lock<MutexType> lock(mutex);
        ptime now(second_clock::universal_time());
        pair<order_by_expire::iterator, order_by_expire::iterator> expiredmsgs = view.range(unbounded, boost::lambda::_1<=now);
        view.erase(expiredmsgs.first, expiredmsgs.second);

        msgExpireTimer.expires_from_now( view.empty() ? minutes(3) : (*view.begin())->ExpireTime - now + seconds(10) );
        msgExpireTimer.async_wait(bind(&Messenger::ExpireTimerHandler, this, placeholders::error));
    }
}

Messenger::MessageRange Messenger::MessageForUser( unsigned int uid, boost::posix_time::ptime msgafter )
{
    typedef MessagesContainer::index<message_ordered_touid_tag>::type order_by_to_user;
    order_by_to_user& view = msgpool.get<message_ordered_touid_tag>();

    MessageRange ret(mutex);

    pair<Messenger::MSGIterator, Messenger::MSGIterator> msgforuser = view.equal_range(uid);

    while (msgforuser.first != msgforuser.second)
        if ((*msgforuser.first)->AddTime <= msgafter)
            ++msgforuser.first;
        else
            break;

    ret._begin = msgforuser.first;
    ret._end = msgforuser.second;
    return ret;
}

void Messenger::MessageConfirmedRecieved(::google::protobuf::uint32 msgid)
{
    typedef MessagesContainer::index<message_hash_msgid_tag>::type hash_by_msgid;
    hash_by_msgid& view = msgpool.get<message_hash_msgid_tag>();

    try
    {
        {
            scoped_lock<MutexType> lock(mutex);
            hash_by_msgid::iterator msg = view.find(msgid);
            view.erase(msg);
        }
        dataadapter->ConfirmMessageDelivered( msgid );
        // msg found
    }
    catch (std::exception* e)
    {
        pantheios::log_WARNING("Error confirming message is delivered(msgid=", pantheios::integer(msgid), "). Error Msg:", e->what());
    }
}

Messenger::MessageRange::MessageRange( const MessageRange& other )
: lock(move(other.lock))
{
    _begin = other._begin;
    _end = other._end;
}

}
}
