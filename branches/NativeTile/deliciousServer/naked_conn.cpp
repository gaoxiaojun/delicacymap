#include "stdafx.h"
#include "naked_conn.h"
#include "deliciousDataAdapter.h"
#include "AsioRpcController.h"
#include "DMServiceLocalDBImpl.h"
#include "Messenger.h"

#include "MapProtocol.pb.h"

#include <exception>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

extern deliciousDataAdapter *dbadapter;

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::posix_time;
using namespace google::protobuf;
using namespace ProtocolBuffer;
using namespace rclib::network;

naked_conn::naked_conn( boost::asio::io_service &io )
:_s(io), msg_timer(io)
{
    controller = new AsioRpcController();
    service = new DMServiceLocalDBImpl(controller);
}

naked_conn::~naked_conn()
{
    delete service;
    delete controller;
}

void naked_conn::prepare()
{
    isClosing = false;
    linked_user = 0;
    message_basetime = time_from_string("1970-01-01 00:00:00");
}

void naked_conn::start()
{
    pantheios::log_INFORMATIONAL("Connection Start. remote machine: ", _s.remote_endpoint().address().to_string(), ":", pantheios::integer(_s.remote_endpoint().port()));
    readrequest(0);
}

void naked_conn::readrequest(int stage)
{
    if (stage == 0)
    {
        _s.async_receive(buffer((void*)&readsize, 4)
            , bind(&naked_conn::readrequest
            , shared_from_this()
            , 1));
    }
    else if (stage == 1)
    {
        _s.async_receive(buffer(inputbuf, ntohl(readsize))
            , bind(&naked_conn::handle_read
            , shared_from_this()
            , placeholders::error
            , placeholders::bytes_transferred));
    }
}

void naked_conn::write( const google::protobuf::MessageLite* msg )
{
    msg->SerializeToString(&outputbuf);
    pantheios::log_INFORMATIONAL("Writing ", pantheios::integer(outputbuf.size()+4), " bytes through tcp connection");
    writesize = (outputbuf.size());
    bufs[0] = buffer((void*)&writesize, 4);
    bufs[1] = buffer(outputbuf);

    _s.async_send(bufs
        , bind(&naked_conn::handle_write
            , shared_from_this()
            , placeholders::error
            , placeholders::bytes_transferred));
}

void naked_conn::writeMessage( const DMessageWrap* m )
{
    outcome.Clear();
    outcome.set_type(protorpc::MESSAGE);
    outcome.set_buffer(m->SerializeAsString());

    outcome.SerializeToString(&outputbuf);
    pantheios::log_INFORMATIONAL("Writing ", pantheios::integer(outputbuf.size()+4), " bytes through tcp connection");
    writesize = (outputbuf.size());
    bufs[0] = buffer((void*)&writesize, 4);
    bufs[1] = buffer(outputbuf);

    _s.async_send(bufs
        , bind(&naked_conn::handle_message_sent
            , shared_from_this()
            , m->msgid()
            , placeholders::error
            , placeholders::bytes_transferred));
}

void naked_conn::close()
{
    pantheios::log_INFORMATIONAL("Connection shutting down.");
    _s.close();
}

void naked_conn::handle_read( const boost::system::error_code& err, size_t bytes_transferred )
{
    if (err)
    {
        pantheios::log_ERROR("read msg err '", err.message() , "' . errcode: ", pantheios::integer(err.value()));
        isClosing = true;
    }
    else
    {
        //handle rpc request and send back reply
        readsize = ntohl(readsize);
        income.ParseFromArray(inputbuf.c_array(), readsize);
        if (income.has_type())
        {
            switch (income.type())
            {
            case protorpc::REQUEST:
                handle_request();
                break;
            case protorpc::MESSAGE:
                handle_messaging();
                break;
            case protorpc::DESCRIPTOR_REQUEST:
            case protorpc::DESCRIPTOR_RESPONSE:
            case protorpc::DISCONNECT:
            case protorpc::RESPONSE_CANCEL:
            case protorpc::RESPONSE_FAILED:
            case protorpc::RESPONSE:
            case protorpc::RESPONSE_NOT_IMPLEMENTED:
            default:
                handle_default();
            }
        }
        else
            pantheios::log_WARNING("Incompatible RPC message. missing 'type'");
    }
}

void naked_conn::handle_write( const boost::system::error_code& err, size_t /*bytes_transferred*/ )
{
    if (err)
    {
        pantheios::log_ERROR("write msg err '", err.message() , "' . errcode: ", pantheios::integer(err.value()));
        isClosing = true;
    }
    else
        readrequest(0);
}

void naked_conn::handle_message_sent( unsigned int msgid, const boost::system::error_code& err, size_t /*bytes_transferred*/ )
{
    if (err)
        pantheios::log_ERROR("Error sending message to client '", err.message() , "' . errcode: ", pantheios::integer(err.value()));
    else
    {
        Messenger::GetInstance()->MessageConfirmedRecieved( msgid );
    }
}

void naked_conn::rpccalldone( google::protobuf::uint32 id, google::protobuf::MessageLite* msg )
{
    outcome.Clear();
    outcome.set_id(id);
    if (controller->Failed())
    {
        outcome.set_type(protorpc::RESPONSE_FAILED);
        outcome.set_buffer(controller->ErrorText());
    }
    else if (controller->IsCanceled())
    {
        outcome.set_type(protorpc::RESPONSE_CANCEL);
        outcome.set_buffer(controller->ErrorText());
    }
    else//success
    {
        outcome.set_type(protorpc::RESPONSE);
        outcome.set_buffer(msg->SerializeAsString());
    }

    write(&outcome);
}

void naked_conn::handle_default()
{
    controller->SetFailed("Operation Not implemented");
    outcome.set_id(income.id());
    outcome.set_type(protorpc::RESPONSE_NOT_IMPLEMENTED);
    outcome.set_name("Operation Not implemented");
    write(&outcome);
}

static inline
MessageLite* ResultTypeForMethod(protorpc::FunctionID method_id)
{
    MessageLite* msg = NULL;
    switch (method_id)
    {
    case protorpc::GetRestaurants:
        msg = new ::ProtocolBuffer::RestaurantList;
        break;
    case protorpc::GetLastestCommentsByUser:
    case protorpc::GetLastestCommentsOfRestaurant:
    case protorpc::GetCommentsOfRestaurantSince:
    case protorpc::GetCommentsOfUserSince:
        msg = new ::ProtocolBuffer::CommentList;
        break;
    case protorpc::UserLogin:
    case protorpc::GetUserInfo:
        msg = new ::ProtocolBuffer::User;
        break;
    case protorpc::GetRelatedUsers:
        msg = new ::ProtocolBuffer::UserList;
        break;
    case protorpc::AddRestaurant:
        msg = new ::ProtocolBuffer::Restaurant;
        break;
    case protorpc::AddCommentForRestaurant:
        msg = new ::ProtocolBuffer::Comment;
        break;
    default:
        pantheios::log_CRITICAL("Not handled method id!!!(Return type)");
    }
    return msg;
}

void naked_conn::handle_request()
{
    if (income.has_id() && income.has_method_id() && income.has_buffer())       //original RPC calls
    {
        if (income.has_buffer())
        {
            query.Clear();
            query.ParseFromString(income.buffer());
            MessageLite* response = ResultTypeForMethod(income.method_id());
            controller->Reset();
            Closure *closure = NewCallback(this, &naked_conn::rpccalldone, income.id(), response);
            controller->NotifyOnCancel(closure);
            try
            {
                service->CallMethod(income.method_id(), controller, &query, response, closure);
                // ugly code, refactor some time else~
                if (income.method_id() == protorpc::UserLogin && !controller->Failed() && !controller->IsCanceled())
                {
                    linked_user = ::google::protobuf::down_cast<::ProtocolBuffer::User*>(response)->uid();
                    msg_timer.expires_from_now(seconds(3));
                    msg_timer.async_wait(bind(&naked_conn::messageTimerHandler, shared_from_this(), placeholders::error));
                }
                delete response;
                response = NULL;
            }
            catch (const std::exception& e)
            {
                pantheios::log_ERROR("Error Calling method ", income.name(), ". Error msg: ", e.what());
                closure->Run();
                delete response;
            }
        }
    }
    else
        pantheios::log_WARNING("Incompatible RPC <request> message. missing 'id' or 'method_id'");
}

void naked_conn::handle_messaging()
{
    if (income.has_buffer())
    {
        message.Clear();
        message.ParseFromString(income.buffer());
        if (message.IsInitialized() && message.fromuser() == linked_user && message.touser() != 0)  // Don't you mess with me. may need more validation here.
        {
            Messenger::GetInstance()->SendMessageToUser(&message);
        }
        else
            pantheios::log_WARNING("Message content not valid. discarding.");
    }
    else
        pantheios::log_WARNING("Incompatible message type. missing buffer");
}

void naked_conn::messageTimerHandler(const boost::system::error_code& err)
{
    if (err)
    {
        pantheios::log_ERROR("Connection timer error, Error message: [", pantheios::integer(err.value()), "] ", err.message());
    }
    else if (!isClosing)
    {
        Messenger::MessageRange msgs = Messenger::GetInstance()->MessageForUser(linked_user, message_basetime);
        BOOST_FOREACH(const DMessageWrap* m, msgs)
        {
            writeMessage(m);
            message_basetime = m->AddTime;
        }
        msg_timer.expires_from_now(seconds(5));
        msg_timer.async_wait(bind(&naked_conn::messageTimerHandler, shared_from_this(), placeholders::error));
    }
}