#include "stdafx.h"
#include "naked_conn.h"
#include "deliciousDataAdapter.h"
#include "AsioRpcController.h"

#include "MapProtocol.pb.h"

#include "google/protobuf/descriptor.h"
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

extern deliciousDataAdapter *dbadapter;

using namespace boost;
using namespace boost::asio;
using namespace google::protobuf;
using namespace ProtocolBuffer;

naked_conn::naked_conn( boost::asio::io_service &io )
:_s(io)
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

}

void naked_conn::start()
{
    pantheios::log_INFORMATIONAL("Connection Start. remote machine: ", _s.remote_endpoint().address().to_string(), ":", pantheios::integer(_s.remote_endpoint().port()));
    readrequest();
}

void naked_conn::readrequest()
{
    array<mutable_buffer, 2> bufs;
    bufs[0] = buffer((void*)&readsize, 4);
    bufs[1] = buffer(inputbuf);
    _s.async_receive(bufs
        , bind(&naked_conn::handle_read
            , shared_from_this()
            , placeholders::error
            , placeholders::bytes_transferred));
}

void naked_conn::write( google::protobuf::Message* msg )
{
    msg->SerializeToString(&outputbuf);
	pantheios::log_INFORMATIONAL("Writing ", pantheios::integer(outputbuf.size()+4), " through tcp connection");
	writesize = (outputbuf.size());
	bufs[0] = buffer((void*)&writesize, 4);
	bufs[1] = buffer(outputbuf);
	_s.async_send(bufs
	    , boost::bind(&naked_conn::handle_write
	        , shared_from_this()
	        , boost::asio::placeholders::error
	        , boost::asio::placeholders::bytes_transferred));
}

void naked_conn::close()
{
    pantheios::log_INFORMATIONAL("Connection shutting down.");
    _s.close();
}

void naked_conn::CallMethod(const MethodDescriptor* method,
                    RpcController* controller,
                    const Message* request,
                    Message* response,
                    Closure* done)
{
    //called by stub(client side), not needed, place a warning just in case
    pantheios::log_EMERGENCY("naked_conn::CallMethod called");
    //request->se
}

void naked_conn::handle_read( const boost::system::error_code& err, size_t bytes_transferred )
{
    if (err)
        pantheios::log_ERROR("read msg err '", err.message() , "' . errcode: ", pantheios::integer(err.value()));
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
        pantheios::log_ERROR("write msg err '", err.message() , "' . errcode: ", pantheios::integer(err.value()));
    else
        readrequest();
}

void naked_conn::rpccalldone( google::protobuf::uint32 id, google::protobuf::Message* msg )
{
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
    delete msg;
}

void naked_conn::handle_default()
{
    controller->SetFailed("Operation Not implemented");
    outcome.set_id(income.id());
    outcome.set_type(protorpc::RESPONSE_NOT_IMPLEMENTED);
    outcome.set_name("Operation Not implemented");
    write(&outcome);
}

void naked_conn::handle_request()
{
    if (income.has_id() && income.has_name())//verify data
    {
        const MethodDescriptor* desc = service->GetDescriptor()->FindMethodByName(income.name());
        if (income.has_buffer())
        {
            query.ParseFromString(income.buffer());
            Message* response = service->GetResponsePrototype(desc).New();
            controller->Reset();
            Closure *closure = NewCallback(this, &naked_conn::rpccalldone, income.id(), static_cast<Message*>(response));
            controller->NotifyOnCancel(closure);
            service->CallMethod(desc, controller, &query, response, closure);
        }
    }
    else
        pantheios::log_WARNING("Incompatible RPC <request> message. missing 'id' or 'name'");
}
