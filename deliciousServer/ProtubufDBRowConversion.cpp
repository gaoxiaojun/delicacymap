#include "StdAfx.h"
#include "ProtubufDBRowConversion.h"
#include "MapProtocol.pb.h"
#include "DBResult.h"

bool ProtubufDBRowConversion::Convert( const DBRow& from, ::ProtocolBuffer::User& to )
{
    to.set_uid(from.GetValueAs<int>("UID"));
    to.set_emailaddress(from["EmailAddress"]);
    to.set_password(from["Password"]);
    to.set_nickname(from["Nickname"]);
    to.mutable_jointime()->set_timestamp(from["JoinTime"]);
    to.mutable_lastlocation()->set_latitude(from.GetValueAs<double>("LastLatitude"));
    to.mutable_lastlocation()->set_longitude(from.GetValueAs<double>("LastLongitude"));

    return true;
}

bool ProtubufDBRowConversion::Convert( const ::ProtocolBuffer::User& from, DBRow& to )
{
    to["EmailAddress"] = from.emailaddress();
    to["Password"] = from.password();
    to["Nickname"] = from.nickname();
    if (from.has_lastlocation())
    {
        to["LastLatitude"] = from.lastlocation().latitude();
        to["LastLongitude"] = from.lastlocation().longitude();
        to["LastUpdatePosition"] = "datetime('now')";  //cheat: let the db get the time
    }

    return true;
}

bool ProtubufDBRowConversion::Convert( const DBRow&from, ::ProtocolBuffer::Restaurant& to )
{
    to.set_name(from["Name"]);
    to.set_commentcount( from["CommentCount"].empty() ? 0 : from.GetValueAs<int>("CommentCount") );
    to.set_rating(from.GetValueAs<int>("Rating"));
    to.set_rid(from.GetValueAs<int>("RID"));
    to.mutable_type()->set_name( from["TID"].empty() ? "" : from["ReadableText"] );
    to.mutable_type()->set_tid( from["TID"].empty() ? -1 : from.GetValueAs<int>("TID") );

    to.mutable_location()->set_longitude(from.GetValueAs<double>("Longtitude"));
    to.mutable_location()->set_latitude(from.GetValueAs<double>("Latitude"));

    to.mutable_averageexpense()->set_amount(from.GetValueAs<float>("AverageExpense"));

    return true;
}

bool ProtubufDBRowConversion::Convert( const DBRow&from, ::ProtocolBuffer::Comment& to )
{
    to.set_content(from["Comment"]);
    to.set_uid(from.GetValueAs<int>("UID"));
    to.set_rid(from.GetValueAs<int>("RID"));
    if (!from["DID"].empty())
        to.set_did(from.GetValueAs<int>("DID"));
    to.mutable_timestamp()->set_timestamp(from["AddTime"]);
//     if (!from["PhotoPath"].empty())
//         to.set_image(from["PhotoPath"]);
    return true;
}
