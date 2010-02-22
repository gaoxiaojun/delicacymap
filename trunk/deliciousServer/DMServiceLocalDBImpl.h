#pragma once
#include "MapProtocol.pb.h"
#include "Message.pb.h"
#include "deliciousDataAdapter.h"
#include <google/protobuf/service.h>

class DMServiceLocalDBImpl
//    : public ProtocolBuffer::DMService
{
public:
    DMServiceLocalDBImpl(google::protobuf::RpcController* controller);
    ~DMServiceLocalDBImpl(void);

    void CallMethod(protorpc::FunctionID method_id,
        google::protobuf::RpcController* controller,
        const google::protobuf::MessageLite* request,
        google::protobuf::MessageLite* response,
        google::protobuf::Closure* done);

///////////////////////////DMService Impl///////////////////////////////////////////////
public:
    /*virtual*/ void GetRestaurants(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::RestaurantList*,
        ::google::protobuf::Closure* done);
    /*virtual*/ void GetLastestCommentsOfRestaurant(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
    /*virtual*/ void GetLastestCommentsByUser(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
    /*virtual*/ void GetCommentsOfUserSince(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
    /*virtual*/ void GetCommentsOfRestaurantSince(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query*,
        ::ProtocolBuffer::CommentList*,
        ::google::protobuf::Closure* done);
	/*virtual*/ void AddCommentForRestaurant(::google::protobuf::RpcController* controller,
		const ::ProtocolBuffer::Query* request,
		::ProtocolBuffer::Comment* response,
		::google::protobuf::Closure* done);
	/*virtual*/ void UserLogin(::google::protobuf::RpcController* controller,
		const ::ProtocolBuffer::Query* request,
		::ProtocolBuffer::User* response,
		::google::protobuf::Closure* done);
	/*virtual*/ void GetUser(::google::protobuf::RpcController* controller,
		const ::ProtocolBuffer::Query* request,
		::ProtocolBuffer::User* response,
		::google::protobuf::Closure* done);
    /*virtual*/ void GetRelatedUsers(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query* request,
        ::ProtocolBuffer::UserList* response,
        ::google::protobuf::Closure* done);
    void UpdateUserInfo(::google::protobuf::RpcController* controller,
        const ::ProtocolBuffer::Query* request,
        ::ProtocolBuffer::User* response,
        ::google::protobuf::Closure* done);

private://callbacks
    void GetRestaurantsCallback( const DBRow& row, ProtocolBuffer::RestaurantList* result );
    void GetCommentsCallback( const DBRow& row, ProtocolBuffer::CommentList* result );
    void GetUsersCallback( const DBRow& row, ProtocolBuffer::UserList* result);
private:
    deliciousDataAdapter* adapter;
    google::protobuf::RpcController* controller;
};
