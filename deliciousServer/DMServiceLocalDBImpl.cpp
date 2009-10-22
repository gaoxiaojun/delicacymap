#include "StdAfx.h"
#include "DMServiceLocalDBImpl.h"
#include "DBResult.h"
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace google::protobuf;
using namespace ProtocolBuffer;
using namespace boost;

DMServiceLocalDBImpl::DMServiceLocalDBImpl(google::protobuf::RpcController* controller)
{
    adapter = deliciousDataAdapter::GetInstance();
    this->controller = controller;
}

DMServiceLocalDBImpl::~DMServiceLocalDBImpl(void)
{
}

void DMServiceLocalDBImpl::GetRestaurants( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::RestaurantList* response, ::google::protobuf::Closure* done )
{
    //validate query
    if (request->has_area() && request->has_level())
    {
		try
		{
			adapter->QueryRestaurantWithinLocation(
				request->area().southwest().longitude(),
				request->area().southwest().latitude(),
				request->area().northeast().longitude(),
				request->area().northeast().latitude(),
				request->level(),
				bind(&DMServiceLocalDBImpl::GetRestaurantsCallback, this, _1, response));
			pantheios::log_DEBUG(response->DebugString());
		}
		catch (const std::exception &e)
		{
			pantheios::log_ERROR(e.what());
			controller->SetFailed("Error calling GetRestaurants().");
		}
    }
    else
    {
        pantheios::log_WARNING("calling GetRestaurants() with wrong request message.");
        controller->SetFailed("calling GetRestaurants() with wrong request message.");
    }
    
    done->Run();
}

void DMServiceLocalDBImpl::GetLastestCommentsOfRestaurant( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::CommentList* response, ::google::protobuf::Closure* done )
{
    if (request->has_rid() && request->has_n())
    {
        adapter->QueryLatestCommentsOfRestaurant(
            request->rid(),
            request->n(),
            bind(&DMServiceLocalDBImpl::GetCommentsCallback, this, _1, response));

        pantheios::log_DEBUG(response->DebugString());
    }
    else
    {
        pantheios::log_WARNING("calling GetLastestCommentsOfRestaurant() with wrong request message.");
        controller->SetFailed("calling GetLastestCommentsOfRestaurant() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetCommentsOfRestaurantSince( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::CommentList* response, ::google::protobuf::Closure* done )
{
    if (request->has_rid() && request->has_time())
    {
        adapter->QueryCommentsOfRestaurantSince(
            request->rid(),
            request->time().timestamp(),
            bind(&DMServiceLocalDBImpl::GetCommentsCallback, this, _1, response));

        pantheios::log_DEBUG(response->DebugString());
    }
    else
    {
        pantheios::log_WARNING("calling GetCommentsOfRestaurantSince() with wrong request message.");
        controller->SetFailed("calling GetCommentsOfRestaurantSince() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetLastestCommentsByUser( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::CommentList* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid() && request->has_n())
    {
        adapter->QueryLastestCommentsByUser(
            request->uid(),
            request->n(),
            bind(&DMServiceLocalDBImpl::GetCommentsCallback, this, _1, response));

        pantheios::log_DEBUG(response->DebugString());
    }
    else
    {
        pantheios::log_WARNING("calling GetLastestCommentsByUser() with wrong request message.");
        controller->SetFailed("calling GetLastestCommentsByUser() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetCommentsOfUserSince( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::CommentList* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid() && request->has_time())
    {
        adapter->QueryCommentsOfUserSince(
            request->uid(),
            request->time().timestamp(),
            bind(&DMServiceLocalDBImpl::GetCommentsCallback, this, _1, response));

        pantheios::log_DEBUG(response->DebugString());
    }
    else
    {
        pantheios::log_WARNING("calling GetCommentsOfUserSince() with wrong request message.");
        controller->SetFailed("calling GetCommentsOfUserSince() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetRestaurantsCallback( const DBRow& row, RestaurantList* result )
{
    Restaurant *newr = result->add_restaurants();
    newr->set_name(row["Name"]);
    newr->set_rating(row.GetValueAs<int>("Rating"));
    newr->set_rid(row.GetValueAs<int>("RID"));
	newr->mutable_type()->set_name(row["ReadableText"]);
	newr->mutable_type()->set_tid(row.GetValueAs<int>("TID"));

    newr->mutable_location()->set_longitude(row.GetValueAs<double>("Longtitude"));
    newr->mutable_location()->set_latitude(row.GetValueAs<double>("Latitude"));

    newr->mutable_averageexpense()->set_amount(row.GetValueAs<double>("AverageExpense"));
}

void DMServiceLocalDBImpl::GetCommentsCallback( const DBRow& row, ProtocolBuffer::CommentList* result )
{
    Comment *newc = result->add_comments();
    newc->set_content(row["Comment"]);
    newc->set_uid(row.GetValueAs<int>("UID"));
    newc->mutable_timestamp()->set_timestamp(row["AddTime"]);
}