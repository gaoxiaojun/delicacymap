#pragma once

#include <boost/asio.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/function.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

#include <set>
#include <map>
#include <unordered_map>
#include <queue>

#include <time.h>

#include "naked_conn.h"
#include "MapProtocol.pb.h"

class deliciousDataAdapter;
class DBRow;

namespace rclib
{
    namespace network
    {
        struct message_hash_fromuid_tag{};

        struct message_hash_msgid_tag{};

        struct message_ordered_touid_tag{};

        struct message_ordered_expiretime_tag{};

        class DMessageWrap : public ProtocolBuffer::DMessage
        {
        public:
            boost::posix_time::ptime AddTime;
            boost::posix_time::ptime ExpireTime;
        };

        struct UserControlBlock
        {
            typedef boost::function<void (const DMessageWrap*)> SenderFunctionProtoType;
            int uid;
            SenderFunctionProtoType senderFunction;
            ProtocolBuffer::Location location;
            std::set<int> shareLocationWith;
            std::queue< std::tr1::weak_ptr<DMessageWrap> > usrMessages;
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
        private:
            typedef _bm::multi_index_container<
                std::tr1::shared_ptr<DMessageWrap>,
                _bm::indexed_by<
                    _bm::hashed_non_unique<
                        _bm::tag<message_hash_fromuid_tag>, 
                        _bm::const_mem_fun<ProtocolBuffer::DMessage, ::google::protobuf::uint32, &ProtocolBuffer::DMessage::fromuser>
                    >,
                    _bm::hashed_unique<
                        _bm::tag<message_hash_msgid_tag>, 
                        _bm::const_mem_fun<ProtocolBuffer::DMessage, ::google::protobuf::uint32, &ProtocolBuffer::DMessage::msgid>
                    >,
                    _bm::ordered_non_unique<
                        _bm::tag<message_ordered_touid_tag>, 
                        _bm::composite_key<
                            DMessageWrap,
                            _bm::const_mem_fun<ProtocolBuffer::DMessage, ::google::protobuf::uint32, &ProtocolBuffer::DMessage::touser>,
                            _bm::member<DMessageWrap, boost::posix_time::ptime, &DMessageWrap::AddTime>
                        >
                    >,
                    _bm::ordered_non_unique<
                        _bm::tag<message_ordered_expiretime_tag>,
                        _bm::member<DMessageWrap, boost::posix_time::ptime, &DMessageWrap::ExpireTime>
                    >
                >
            > MessagesContainer;
            typedef boost::interprocess::interprocess_upgradable_mutex MutexType;
            typedef std::tr1::unordered_map<int, UserControlBlock> UserContainer;
        public:
            typedef MessagesContainer::index<message_ordered_touid_tag>::type::iterator MSGIterator;
            typedef MessagesContainer::index<message_ordered_touid_tag>::type::const_iterator const_MSGIterator;

            // A simple enhancement to std::pair to help managing concurrency
            class MessageRange
            {
            public:
                typedef const_MSGIterator iterator;
                typedef const_MSGIterator const_iterator;
                const_MSGIterator _begin, _end;
                const_MSGIterator begin() const {return _begin;}
                const_MSGIterator end() const {return _end;}

                MessageRange(MutexType &mutex) : lock(mutex){}
                MessageRange(const MessageRange& other);
            private:
                mutable boost::interprocess::sharable_lock<MutexType> lock;
            };

            static void Initialize(boost::asio::io_service &);
            static Messenger* GetInstance();

            ~Messenger(void);

            void start();

            bool RegisterUserOnConnection( int uid, UserControlBlock::SenderFunctionProtoType func);

            void SignOffUser( int uid );

            void ProcessMessage(ProtocolBuffer::DMessage* msg);

            MessageRange MessageForUser(unsigned int, boost::posix_time::ptime);
            void MessageConfirmedRecieved(::google::protobuf::uint32);

        protected:
            Messenger(boost::asio::io_service &);
            bool ProcessSystemMessage( ProtocolBuffer::DMessage*);

        private: //call back
            void GetMessageCallback( const DBRow& row );

            void ExpireTimerHandler(const boost::system::error_code& err);

            void SendMessageHandler();

        private:
            MutexType pool_mutex, usr_mutex;
            UserContainer liveUsers;
            boost::asio::deadline_timer msgExpireTimer, msgSendTimer;
            boost::asio::io_service &ios;
            ::deliciousDataAdapter *dataadapter;
            MessagesContainer msgpool;

            static Messenger* __messenger;
        };
    }
}
