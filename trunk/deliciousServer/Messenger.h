#pragma once

#include <boost/asio.hpp>
#include <boost/multi_index_container_fwd.hpp>
#include <boost/multi_index/hashed_index_fwd.hpp>
#include <boost/multi_index/ordered_index_fwd.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <time.h>

#include "MapProtocol.pb.h"

class deliciousDataAdapter;
class DBRow;

namespace rclib
{
    namespace network
    {
        struct message_hash_fromuid_tag{};

        struct message_hash_touid_tag{};

        struct message_ordered_expiretime_tag{};

        class DMessageWrap : public ProtocolBuffer::DMessage
        {
        public:
            boost::posix_time::ptime AddTime;
            boost::posix_time::ptime ExpireTime;
        };

        namespace _bm=boost::multi_index;


        //Requires the DB to be initialized first.

        // Goal of Messenger class is to manage message delivery for all users as little access to DB as possible.
        // It supports:
        // Maintaining all user messages and its states
        // dispose a message(still keeps it in DB) once it is delivered
        // dispose a message if it expires.
        // deliver message from user to user without actively polling the DB
        class Messenger
        {
        public:
            Messenger(boost::asio::io_service &);
            ~Messenger(void);

            void start();

            void SendMessageToUser(ProtocolBuffer::DMessage* msg);

        private: //call back
            void GetMessageCallback( const DBRow& row );

            void ExpireTimerHandler();

        private:
            typedef _bm::multi_index_container<
                DMessageWrap,
                _bm::indexed_by<
                    _bm::hashed_non_unique<
                        _bm::tag<message_hash_fromuid_tag>, 
                        _bm::const_mem_fun<ProtocolBuffer::DMessage, ::google::protobuf::uint32, &ProtocolBuffer::DMessage::fromuser>
                    >,
                    _bm::hashed_non_unique<
                        _bm::tag<message_hash_touid_tag>, 
                        _bm::const_mem_fun<ProtocolBuffer::DMessage, ::google::protobuf::uint32, &ProtocolBuffer::DMessage::touser>
                    >
//                     _bm::ordered_non_unique<
//                         _bm::tag<message_ordered_expiretime_tag>,
//                         _bm::member<DMessageWrap, tm, &DMessageWrap::ExpireTime>
//                     >
                >
            > MessagesContainer;

            boost::asio::deadline_timer msgExpireTimer;
            boost::asio::io_service &ios;
            ::deliciousDataAdapter *dataadapter;
            MessagesContainer *msgpool;
        };
    }
}
