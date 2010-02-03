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
        to["LastUpdatePosition"] = "from('now')";  //cheat: let the db get the time
    }

    return true;
}
