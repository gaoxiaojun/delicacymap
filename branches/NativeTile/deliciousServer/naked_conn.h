#pragma once

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/array.hpp>
#include <google/protobuf/service.h>

#include "Message.pb.h"

const size_t MaxInputBufSize = 1024 * 1024;

class AsioRpcController;
class DMServiceLocalDBImpl;

namespace rclib{
    namespace network{
        class DMessageWrap;
    }
}

class naked_conn
    : public boost::enable_shared_from_this<naked_conn>
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

private://methods
    void readrequest(int);

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& err, size_t /*bytes_transferred*/);
    void handle_message_sent(unsigned int msgid, const boost::system::error_code& err, size_t /*bytes_transferred*/);
    void handle_messaging();

    void rpccalldone( google::protobuf::uint32 id, google::protobuf::MessageLite* msg );

    void messageTimerHandler(const boost::system::error_code& err);

private://methods handling different rpc request types
    void handle_request();

    void handle_default();

    void write(const google::protobuf::MessageLite* msg);

    void writeMessage(const rclib::network::DMessageWrap*);
private://data
    boost::asio::ip::tcp::socket _s;
    boost::array<char, MaxInputBufSize> inputbuf;
	boost::array<boost::asio::const_buffer, 2> bufs;
    ProtocolBuffer::Query query;
    ProtocolBuffer::DMessage message;
    protorpc::Message income, outcome;
    boost::asio::deadline_timer msg_timer;
    boost::posix_time::ptime message_basetime;
    std::string outputbuf;
    DMServiceLocalDBImpl *service;
    AsioRpcController *controller;
    size_t readsize, writesize;
    unsigned int linked_user;
    bool isClosing;
};