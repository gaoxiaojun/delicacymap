#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <google/protobuf/service.h>

#include "Message.pb.h"
#include "DMServiceLocalDBImpl.h"

const size_t MaxInputBufSize = 1024 * 1024;

class AsioRpcController;

class naked_conn
    : public boost::enable_shared_from_this<naked_conn>
    , public google::protobuf::RpcChannel
    , private boost::noncopyable
{
public://typedefs
    //friend class ConnectionPool<RequestHandler, ReplyHandler>;
    typedef boost::shared_ptr<naked_conn> pointer;

public://constructors
    naked_conn(boost::asio::io_service &io);
    ~naked_conn();

public://methods
    void prepare();

    void start();

    void close();

    boost::asio::ip::tcp::socket& socket() {return _s;}

    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller,
        const google::protobuf::Message* request,
        google::protobuf::Message* response,
        google::protobuf::Closure* done);

private://methods
    void readrequest();

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& err, size_t /*bytes_transferred*/);

    void rpccalldone( google::protobuf::uint32 id, google::protobuf::Message* msg );

private://methods handling different rpc request types
    void handle_request();

    void handle_default();

    void write(google::protobuf::Message* msg);
private://data
    boost::asio::ip::tcp::socket _s;
    boost::array<char, MaxInputBufSize> inputbuf;
	boost::array<boost::asio::const_buffer, 2> bufs;
    ProtocolBuffer::Query query;
    protorpc::Message income, outcome;
    ProtocolBuffer::DMService *service;
    AsioRpcController *controller;
    size_t readsize, writesize;
	std::string outputbuf;
};