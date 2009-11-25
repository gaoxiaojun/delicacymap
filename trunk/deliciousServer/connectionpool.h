#ifndef __CONNECTIONPOOL__H__INCLUDED__
#define __CONNECTIONPOOL__H__INCLUDED__

//#include "http_handlers.h"

#include <cassert>
#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

namespace rclib
{
    namespace network
    {
        //template <typename RequestHandler, typename ReplyHandler>
        //class ConnectionPool;

        template <typename ConnectionT>
        class ConnectionPool
        {
            public:
                typedef std::vector<ConnectionT*> POOLCONT;
            public:
                ConnectionPool(boost::asio::io_service& io)
                    :_used(0),_io(io){}
                ~ConnectionPool()
                {
                    BOOST_FOREACH(ConnectionT* ptr, _pool)
                        delete ptr;
                }
                typename ConnectionT::pointer getavailconnection()
                {
                    boost::lock_guard<boost::mutex> lock(_m);
                    if (_used == _pool.size())
                        expand();
                    return typename ConnectionT::pointer(_pool[_used++], boost::bind(&ConnectionPool::returntopool, this, _1));
                }
            private:
                void returntopool(ConnectionT* conn)
                {
                    assert(conn != NULL);
                    conn->close();
                    typename POOLCONT::iterator iter = std::find(_pool.begin(),_pool.end(), conn);
                    assert(iter != _pool.end());
                    boost::lock_guard<boost::mutex> lock(_m);
                    std::swap(_pool.at(--_used), *iter);
                }
            private:
                void expand()
                {
                    for (size_t i=0;i<128;++i)
                    {
                        _pool.push_back(new ConnectionT(_io));
                    }
                }
            private:
                boost::mutex _m;
                POOLCONT _pool;
                size_t _used;
                boost::asio::io_service& _io;
        };
    }
}

#endif
