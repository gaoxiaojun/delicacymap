#include "stdafx.h"
#include "DMServiceLocalDBImpl.h"
#include "DBResult.h"
#include "ProtubufDBRowConversion.h"
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

void DMServiceLocalDBImpl::CallMethod( protorpc::FunctionID method_id, google::protobuf::RpcController* controller, const google::protobuf::MessageLite* request, google::protobuf::MessageLite* response, google::protobuf::Closure* done )
{
    switch (method_id)
    {
    case protorpc::GetRestaurants:
        GetRestaurants(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::RestaurantList*>(response),
            done);
        break;
    case protorpc::GetLastestCommentsOfRestaurant:
        GetLastestCommentsOfRestaurant(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::CommentList*>(response),
            done);
        break;
    case protorpc::GetLastestCommentsByUser:
        GetLastestCommentsByUser(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::CommentList*>(response),
            done);
        break;
    case protorpc::GetCommentsOfUserSince:
        GetCommentsOfUserSince(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::CommentList*>(response),
            done);
        break;
    case protorpc::GetCommentsOfRestaurantSince:
        GetCommentsOfRestaurantSince(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::CommentList*>(response),
            done);
        break;
    case protorpc::UserLogin:
        UserLogin(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::User*>(response),
            done);
        break;
    case protorpc::GetUserInfo:
        GetUser(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::User*>(response),
            done);
        break;
    case protorpc::UpdateUserInfo:
        UpdateUserInfo(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::User*>(response),
            done);
        break;
    case protorpc::GetRelatedUsers:
        GetRelatedUsers(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::UserList*>(response),
            done);
        break;
    case protorpc::AddCommentForRestaurant:
        AddCommentForRestaurant(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::Comment*>(response),
            done);
        break;
    case protorpc::AddRestaurant:
//         AddRestaurant(controller,
//             ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
//             ::google::protobuf::down_cast< ::ProtocolBuffer::Restaurant*>(response),
//             done);
//         break;
    default:
        pantheios::log_CRITICAL("Not handled method id!!!(CallMethod)");
    }
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
            //pantheios::log_DEBUG(response->Utf8DebugString());
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

        //pantheios::log_DEBUG(response->DebugString());
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

        //pantheios::log_DEBUG(response->DebugString());
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

        //pantheios::log_DEBUG(response->DebugString());
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

        //pantheios::log_DEBUG(response->DebugString());
    }
    else
    {
        pantheios::log_WARNING("calling GetCommentsOfUserSince() with wrong request message.");
        controller->SetFailed("calling GetCommentsOfUserSince() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::AddCommentForRestaurant( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::Comment* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid() && request->has_rid() && request->has_msg())
    {
        DBResultWrap ret = adapter->PostCommentForRestaurant( 
            request->rid(), 
            request->uid(),
            request->msg(),
            request->has_image() ? &request->image() : NULL);

        if (!ret.empty())
        {
            const DBRow& newcomment = ret.getResult()->GetRow(0);
            response->set_rid( newcomment.GetValueAs<int>("RID") );
            response->set_uid( newcomment.GetValueAs<int>("UID") );
            if (!newcomment["DID"].empty())
                response->set_did( newcomment.GetValueAs<int>("DID") );
            response->set_content( newcomment["Comment"] );
            response->mutable_timestamp()->set_timestamp( newcomment["AddTime"] );
//             if (!newcomment["PhotoPath"].empty())
//                 response->set_image(newcomment["PhotoPath"]);
        }
        else
        {
            pantheios::log_WARNING("AddCommentForRestaurant() did not return any results, SQLite error?.");
            controller->SetFailed("Add Comment failed.");
        }
    }
    else
    {
        pantheios::log_WARNING("calling AddCommentForRestaurant() with wrong request message.");
        controller->SetFailed("calling AddCommentForRestaurant() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::UserLogin( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::User* response, ::google::protobuf::Closure* done )
{
    if (request->has_password() && request->has_emailaddress())
    {
        DBResultWrap ret = adapter->UserLogin(request->emailaddress(), request->password());
        if (!ret.empty())
        {
            const DBRow& usr = ret.getResult()->GetRow(0);

            ProtubufDBRowConversion::Convert(usr, *response);
            // preferTypes and friends is not implemented yet.
        }
        else
        {
            pantheios::log_INFORMATIONAL("Login failed. usr=", request->emailaddress(), ", pwd=", request->password(), ".");
            controller->SetFailed("Login filed, username password mismatch.");
        }
    }
    else
    {
        pantheios::log_WARNING("calling UserLogin() with wrong request message.");
        controller->SetFailed("calling UserLogin() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetUser( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::User* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid())
    {
        DBResultWrap ret = adapter->GetUserInfo(request->uid());
        if (!ret.empty())
        {
            const DBRow& usr = ret.getResult()->GetRow(0);

            ProtubufDBRowConversion::Convert(usr, *response);
            response->set_password("");
            // preferTypes and friends is not implemented yet.
        }
        else
        {
            pantheios::log_INFORMATIONAL("No user exist whose uid = ", pantheios::integer(request->uid()), ".");
            controller->SetFailed("User do not exist.");
        }
    }
    else
    {
        pantheios::log_WARNING("calling GetUser() with wrong request message.");
        controller->SetFailed("calling GetUser() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::UpdateUserInfo( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::User* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid() && request->has_password() && request->has_userinfo())
    {
        DBResultWrap usrrow = adapter->GerUserAfterValidation(request->uid(), request->password());
        if (!usrrow.empty())
        {
            const ProtocolBuffer::User &toupdate = request->userinfo();
            DBRow& row = usrrow.getResult()->GetRow(0);
            
            ProtubufDBRowConversion::Convert(toupdate, row);

            DBResultWrap updatedUser = adapter->UpdateRows(usrrow, "Users", "UID");

            if (!updatedUser.empty())
                ProtubufDBRowConversion::Convert(updatedUser.getResult()->GetRow(0), *response);
        }
        else
        {
            pantheios::log_INFORMATIONAL("UpdateUserInfo() failed because user do not exist or authentication failed.");
        }
    }
    else
    {
        pantheios::log_WARNING("calling UpdateUserInfo() with wrong request message.");
        controller->SetFailed("calling UpdateUserInfo() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetRelatedUsers( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::UserList* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid() && request->has_relation())
    {
        adapter->GetRelatedUsersWith(
            request->uid(),
            request->relation(),
            bind(&DMServiceLocalDBImpl::GetUsersCallback, this, _1, response));
    }
    else
    {
        pantheios::log_WARNING("calling GetRelatedUser() with wrong request message.");
        controller->SetFailed("calling GetRelatedUser() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetRestaurantsCallback( const DBRow& row, RestaurantList* result )
{
    Restaurant *newr = result->add_restaurants();
    newr->set_name(row["Name"]);
    newr->set_commentcount(row.GetValueAs<int>("CommentCount"));
    newr->set_rating(row.GetValueAs<int>("Rating"));
    newr->set_rid(row.GetValueAs<int>("RID"));
    newr->mutable_type()->set_name(row["ReadableText"]);
    newr->mutable_type()->set_tid(row.GetValueAs<int>("TID"));

    newr->mutable_location()->set_longitude(row.GetValueAs<double>("Longtitude"));
    newr->mutable_location()->set_latitude(row.GetValueAs<double>("Latitude"));

    newr->mutable_averageexpense()->set_amount(row.GetValueAs<float>("AverageExpense"));
}

void DMServiceLocalDBImpl::GetCommentsCallback( const DBRow& row, ProtocolBuffer::CommentList* result )
{
    Comment *newc = result->add_comments();
    newc->set_content(row["Comment"]);
    newc->set_uid(row.GetValueAs<int>("UID"));
    newc->set_rid(row.GetValueAs<int>("RID"));
    if (!row["DID"].empty())
        newc->set_did(row.GetValueAs<int>("DID"));
    newc->mutable_timestamp()->set_timestamp(row["AddTime"]);
//     if (!row["PhotoPath"].empty())
//         newc->set_image(row["PhotoPath"]);
    ProtubufDBRowConversion::Convert(row, *newc->mutable_userinfo());
}

void DMServiceLocalDBImpl::GetUsersCallback( const DBRow& row, ProtocolBuffer::UserList* result )
{
    User* newuser = result->add_users();
    ProtubufDBRowConversion::Convert(row, *newuser);
}
