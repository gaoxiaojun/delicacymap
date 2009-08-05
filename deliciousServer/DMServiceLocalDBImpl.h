#pragma once
#include "MapProtocol.pb.h"
#include "deliciousDataAdapter.h"

class DMServiceLocalDBImpl
    : public ProtocolBuffer::DMService
{
public:
    DMServiceLocalDBImpl(google::protobuf::RpcController* controller);
    ~DMServiceLocalDBImpl(void);

///////////////////////////DMService Impl///////////////////////////////////////////////
public:
    virtual void GetRestaurants(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::RestaurantList*,
        ::google::protobuf::Closure* done);
    virtual void GetLastestCommentsOfRestaurant(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
    virtual void GetLastestCommentsByUser(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
    virtual void GetCommentsOfUserSince(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
    virtual void GetCommentsOfRestaurantSince(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);

private://callbacks
    void GetRestaurantsCallback( const DBRow& row, ProtocolBuffer::RestaurantList* result );
    void GetCommentsCallback( const DBRow& row, ProtocolBuffer::CommentList* result );
private:
    deliciousDataAdapter* adapter;
    google::protobuf::RpcController* controller;
};
