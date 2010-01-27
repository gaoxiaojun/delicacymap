#include "stdafx.h"
#include "deliciousDataAdapter.h"

#include "DBContext.h"
#include "DBResult.h"

#include <pantheios/inserters/real.hpp>

#include <exception>

using namespace std;
using namespace ProtocolBuffer;

deliciousDataAdapter* deliciousDataAdapter::_single = NULL;

deliciousDataAdapter::deliciousDataAdapter(const std::string& connstr)
{
    try
    {
        dbconn = NULL;
        dbconn = new DBContext(connstr); 
        DBResult *ret = dbconn->Execute("PRAGMA foreign_keys = true");
        dbconn->Free(&ret);
    }
    catch (exception&)
    {
        delete dbconn;
        dbconn = NULL;
        throw;
    }
}

deliciousDataAdapter::~deliciousDataAdapter(void)
{
    delete dbconn;
}

deliciousDataAdapter* deliciousDataAdapter::GetInstance()
{
    if (!_single)
        Initialize("delicacyDB.db3"); //TODO: give more rational solution.
    return _single;
}

void deliciousDataAdapter::Initialize(const string& dbfile)
{
    _single = new deliciousDataAdapter(dbfile);
}

DBResultWrap::DBResultWrap( DBResult* result, DBContext* context )
{
    this->result = result;
    this->context = context;
}

DBResultWrap::DBResultWrap( const DBResultWrap& rhs )
{
    this->result = rhs.result;
    rhs.result = NULL;
    this->context = rhs.context;
    rhs.context = NULL;
}
DBResultWrap::~DBResultWrap()
{
    context->Free(&result);
}

DBResult* DBResultWrap::getResult() const
{
    return result;
}

bool DBResultWrap::empty() const
{
    return result == NULL || getResult()->RowsCount() == 0;
}

void deliciousDataAdapter::ExecuteNormal( char* query, CallbackFunc callback )
{
    DBResult* ret = dbconn->Execute(query);

    for (size_t i=0;i<ret->RowsCount();++i)
        callback(ret->GetRow(i));

    dbconn->Free(&ret);
}

size_t deliciousDataAdapter::QueryRestaurantWithinLocation( double longtitude_from, double latitude_from, double lontitude_to, double latitude_to, int level, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryRestaurantWithinLocation(",
        "longtitude_from=", pantheios::real(longtitude_from),
        ",latitude_from=", pantheios::real(latitude_from),
        ",longtitue_to=", pantheios::real(lontitude_to),
        ",latitude_to=", pantheios::real(latitude_to),
        ",level=", pantheios::integer(level),
        ")");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT Restaurants.*, count(cid) as CommentCount, RestaurantTypes.* "
        "FROM Restaurants NATURAL INNER JOIN Relation_Restaurant_RestaurantType NATURAL INNER JOIN RestaurantTypes LEFT OUTER JOIN comments ON Restaurants.rid = Comments.rid "
        "WHERE Longtitude BETWEEN %.10f AND %.10f "
        "AND Latitude BETWEEN %.10f AND %.10f "
        "GROUP BY rid"
        , longtitude_from, lontitude_to
        , latitude_from, latitude_to);

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryLatestCommentsOfRestaurant( int rid, int n, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryLatestCommentsOfRestaurant(",
        "rid=", pantheios::integer(rid),
        ",n=", pantheios::integer(n),
        ")");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT Comments.* "
        "FROM Comments "
        "WHERE rid=%d "
        "ORDER BY addtime "
        "LIMIT %d;"
        , rid
        , n);

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryCommentsOfRestaurantSince( int rid, const std::string& timestamp, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryCommentsOfRestaurantSince(",
        "rid=", pantheios::integer(rid),
        ",timestamp='", timestamp,
        "')");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT Comments.* "
        "FROM Relation_Restaurant_Comment NATURAL INNER JOIN Comments "
        "WHERE rid=%d AND addtime >= '%s';"
        , rid
        , timestamp.c_str());

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryLastestCommentsByUser( int uid, int n, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryLastestCommentsByUser(",
        "uid=", pantheios::integer(uid),
        ",n=", pantheios::integer(n),
        ")");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Comments "
        "WHERE uid=%d "
        "ORDER BY addtime "
        "LIMIT %d;"
        , uid
        , n);

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryCommentsOfUserSince( int uid, const std::string& timestamp, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryCommentsOfRestaurantSince(",
        "uid=", pantheios::integer(uid),
        ",timestamp='", timestamp,
        "')");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Comments "
        "WHERE uid == %d AND addtime >= '%s' "
        "ORDER BY addtime;"
        , uid
        , timestamp.c_str());

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryMessagesToUser( int uid, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryMessagesToUser(",
        "uid=", pantheios::integer(uid),
        "')");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Messages "
        "WHERE ToUid == %d AND  ExpireTime < CURRENT_TIMESTAMP AND Delivered == 0"
        "ORDER BY ExpireTime;"
        , uid);

    ExecuteNormal(querystr, callback);
    return 0;
}

const DBResultWrap deliciousDataAdapter::PostCommentForRestaurant( int rid, int uid, const std::string& msg, const std::string* const image )
{
    pantheios::log_INFORMATIONAL("PostCommentForRestaurant(",
        "rid=", pantheios::integer(rid),
        ",uid=", pantheios::integer(uid),
        ",msg=", msg,
        "')");
    // Validate user Input!!!!!
    // user input, query might be very long.
    char querystr[4096];
    // single query run atomicity in sqlite, so this has no problem.
    sprintf_s(querystr, sizeof(querystr),
        "INSERT INTO Comments (UID, RID, Comment) VALUES(%d, %d, \"%s\");"
        "SELECT * FROM Comments WHERE Comments.rowid = (select last_insert_rowid());"
        , uid
        , rid
        , msg.c_str());
    return DBResultWrap(dbconn->Execute(querystr), dbconn);
}

const DBResultWrap deliciousDataAdapter::UserLogin( const std::string& email, const std::string& password )
{
    pantheios::log_INFORMATIONAL("UserLogin(",
        "emailAddress=", email,
        ",password=", password,
        ")");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Users "
        "WHERE EmailAddress = '%s';"
        , email.c_str()
        );
    DBResult* ret = dbconn->Execute(querystr);
    if (ret->RowsCount() != 1 || (*ret)[0]["Password"] != password) // Email is unique, so only 0 or 1 row.
    {
        dbconn->Free(&ret);
    }
    return DBResultWrap(ret, dbconn);
}
