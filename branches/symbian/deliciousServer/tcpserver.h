#ifndef __TCPSERVER__H__INCLUDED__
#define __TCPSERVER__H__INCLUDED__

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>
#include "connectionpool.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace rclib
{
    namespace network
    {
        template <typename ConnectionT>
        class TCPServer
        {
        public:
            //  构造函数
            //  参数：  可用的boost::io_service对象
            TCPServer(boost::asio::io_service &io, int port)
                :_acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
                ,_connpool(io)
                ,_port(port)
            {
                boost::asio::ip::tcp::resolver resolver(io);
                boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(),"");
                boost::asio::ip::tcp::resolver::iterator it=resolver.resolve(query);

                while(it!=boost::asio::ip::tcp::resolver::iterator())
                {
                    boost::asio::ip::address addr=(it++)->endpoint().address();
                    if(addr.is_v6())
                        pantheios::log_INFORMATIONAL("ipv6 address: ", addr.to_string());
                    else
                        pantheios::log_INFORMATIONAL("ipv4 address: ", addr.to_string());
                }
                pantheios::log_INFORMATIONAL("Binding at: ", _acceptor.local_endpoint().address().to_string(), ":", pantheios::integer(_acceptor.local_endpoint().port()));
            };
            //  开始服务器的运行
            //  无参数
            void start()
            {
                typename ConnectionT::pointer conn = _connpool.getavailconnection();
                _acceptor.async_accept(conn->socket()
                    , boost::bind(&TCPServer::handle_accept, this, conn, boost::asio::placeholders::error));
            }
            void stop();
        private:
            typedef ConnectionPool<ConnectionT> ConnPool;
            //  客户端连入服务器
            //  参数：  ptr:客户端连接的对象指针
            //          err:可能出现的错误
            void handle_accept(typename ConnectionT::pointer ptr, const boost::system::error_code &err)
            {
                //std::cout<<"Thead "<<boost::this_thread::get_id()<<" is handling acceptence of "<<ptr->socket().remote_endpoint()<<std::endl;
                if (err)
                {
                    pantheios::log_ERROR( err.message() );
                }
                else
                {
                    start();
                    ptr->prepare();
                    ptr->start();
                }
            }
        private:
            const int _port;
            boost::asio::ip::tcp::acceptor _acceptor;
        public:
            ConnPool _connpool;
        };
    }
}

#endif
