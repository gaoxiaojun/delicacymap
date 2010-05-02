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
    adapter = deliciousDataAdapter::NewInstance();
    this->controller = controller;
}

DMServiceLocalDBImpl::~DMServiceLocalDBImpl(void)
{
    delete adapter;
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
    case protorpc::RegisterUser:
        RegisterUser(controller,
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
    case protorpc::SetUserRelation:
        SetUserRelation(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            done);
        break;
    case protorpc::AddRestaurant:
        AddRestaurant(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::Restaurant*>(response),
            done);
        break;
    case protorpc::Search:
        Search(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::SearchResult*>(response),
            done);
        break;
    case protorpc::GetSubscribtionInfo:
        GetSubscribtionInfo(controller,
            ::google::protobuf::down_cast<const ::ProtocolBuffer::Query*>(request),
            ::google::protobuf::down_cast< ::ProtocolBuffer::SearchResult*>(response),
            done);
        break;
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
            ProtubufDBRowConversion::Convert(newcomment, *response);
            ProtubufDBRowConversion::Convert(newcomment, *response->mutable_userinfo());
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
            controller->SetFailed("Username password mismatch.");
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
        DBResultWrap usrrow = adapter->GetUserAfterValidation(request->uid(), request->password());
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

void DMServiceLocalDBImpl::SetUserRelation( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::google::protobuf::Closure* done )
{
    if (request->has_uid() && request->has_uid_target() && request->has_relation())
    {
        if (!adapter->SetUserRelation(request->uid(), request->uid_target(), request->relation()))
        {
            controller->SetFailed("Set user relation failed");
        }
    }
    else
    {
        pantheios::log_WARNING("calling SetUserRelation() with wrong request message.");
        controller->SetFailed("calling SetUserRelation() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::AddRestaurant( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::Restaurant* response, ::google::protobuf::Closure* done )
{
    if (request->has_rinfo())
    {
        DBResultWrap ret = adapter->AddRestaurant(request->rinfo().name(), 
            request->rinfo().location().latitude(), 
            request->rinfo().location().longitude(), 
            request->rinfo().type().tid(),
            request->rinfo().averageexpense().amount());
        if (ret.empty())
        {
            pantheios::log_ERROR("Add restaurant error, database failed?");
            controller->SetFailed("Server failed to add the restaurant.");
        }
        else
            ProtubufDBRowConversion::Convert(ret.getResult()->GetRow(0), *response);
    }
    else
    {
        pantheios::log_WARNING("calling AddRestaurant() with wrong request message.");
        controller->SetFailed("calling AddRestaurant() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::Search( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::SearchResult* response, ::google::protobuf::Closure* done )
{
    if (request->has_msg())
    {
        DBResultWrap ret = adapter->SearchRestaurant(request->msg());
        if (!ret.empty())
        {
            for (size_t i=0;i<ret.getResult()->RowsCount();i++)
            {
                ProtocolBuffer::Restaurant* newr = response->mutable_restaurants()->add_restaurants();
                ProtubufDBRowConversion::Convert(ret.getResult()->GetRow(i), *newr);
            }
        }
        DBResultWrap retuser = adapter->SearchUser(request->msg());
        if (!retuser.empty())
        {
            for (size_t i=0;i<retuser.getResult()->RowsCount();i++)
            {
                ProtocolBuffer::User* newu = response->mutable_users()->add_users();
                ProtubufDBRowConversion::Convert(retuser.getResult()->GetRow(i), *newu);
            }
        }
    }
    else
    {
        pantheios::log_WARNING("calling SearchRestaurant() with wrong request message.");
        controller->SetFailed("calling SearchRestaurant() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetSubscribtionInfo( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::SearchResult* response, ::google::protobuf::Closure* done )
{
    if (request->has_uid())
    {
        DBResultWrap retuser = adapter->GetSubscribedUserBy(request->uid());
        DBResultWrap retrestaurant = adapter->GetSubscribedRestaurantBy(request->uid());

        if (!retuser.empty())
        {
            ProtocolBuffer::UserList* rs = response->mutable_users();
            for (size_t i=0;i<retuser.getResult()->RowsCount();i++)
            {
                ProtubufDBRowConversion::Convert(retuser.getResult()->GetRow(i), *rs->add_users());
            }
        }
        if (!retrestaurant.empty())
        {
            ProtocolBuffer::RestaurantList* us = response->mutable_restaurants();
            for (size_t i=0;i<retrestaurant.getResult()->RowsCount();i++)
            {
                ProtubufDBRowConversion::Convert(retrestaurant.getResult()->GetRow(i), *us->add_restaurants());
            }
        }
    }
    else
    {
        pantheios::log_WARNING("calling GetSubscribtionInfo() with wrong request message.");
        controller->SetFailed("calling GetSubscribtionInfo() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::RegisterUser( ::google::protobuf::RpcController* controller, const ::ProtocolBuffer::Query* request, ::ProtocolBuffer::User* response, ::google::protobuf::Closure* done )
{
    if (request->has_emailaddress() && request->has_password() && request->has_nickname())
    {
        DBResultWrap usr = adapter->GetUserInfo(request->emailaddress());
        if (!usr.empty())
        {
            controller->SetFailed("User already exist.");
        }
        else
        {
            DBResultWrap newUsr = adapter->AddUser(request->nickname(), request->emailaddress(), request->password());
            ProtubufDBRowConversion::Convert(newUsr.getResult()->GetRow(0), *response);
        }
    }
    else
    {
        pantheios::log_WARNING("calling RegisterUser() with wrong request message.");
        controller->SetFailed("calling RegisterUser() with wrong request message.");
    }

    done->Run();
}

void DMServiceLocalDBImpl::GetRestaurantsCallback( const DBRow& row, RestaurantList* result )
{
    Restaurant *newr = result->add_restaurants();

    ProtubufDBRowConversion::Convert(row, *newr);
}

void DMServiceLocalDBImpl::GetCommentsCallback( const DBRow& row, ProtocolBuffer::CommentList* result )
{
    Comment *newc = result->add_comments();
    ProtubufDBRowConversion::Convert(row, *newc);
    ProtubufDBRowConversion::Convert(row, *newc->mutable_userinfo());
}

void DMServiceLocalDBImpl::GetUsersCallback( const DBRow& row, ProtocolBuffer::UserList* result )
{
    User* newuser = result->add_users();
    ProtubufDBRowConversion::Convert(row, *newuser);
}
