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
            //  ���캯��
            //  ������  ���õ�boost::io_service����
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
            //  ��ʼ������������
            //  �޲���
            void start()
            {
                typename ConnectionT::pointer conn = _connpool.getavailconnection();
                _acceptor.async_accept(conn->socket()
                    , boost::bind(&TCPServer::handle_accept, this, conn, boost::asio::placeholders::error));
            }
            void stop();
        private:
            typedef ConnectionPool<ConnectionT> ConnPool;
            //  �ͻ������������
            //  ������  ptr:�ͻ������ӵĶ���ָ��
            //          err:���ܳ��ֵĴ���
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
