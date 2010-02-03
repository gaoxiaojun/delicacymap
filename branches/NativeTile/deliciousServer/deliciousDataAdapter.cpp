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

size_t deliciousDataAdapter::ExecuteNormal( char* query, CallbackFunc callback )
{
    DBResult* ret = dbconn->Execute(query);

    size_t rows = ret->RowsCount();

    for (size_t i=0;i<rows;++i)
        callback(ret->GetRow(i));

    dbconn->Free(&ret);

    return rows;
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

    return ExecuteNormal(querystr, callback);
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

    return ExecuteNormal(querystr, callback);
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

    return ExecuteNormal(querystr, callback);
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

    return ExecuteNormal(querystr, callback);
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

    return ExecuteNormal(querystr, callback);
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
        "SELECT * FROM Comments WHERE Comments.rowid = last_insert_rowid();"
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

const DBResultWrap deliciousDataAdapter::GerUserAfterValidation( int uid, const std::string& password )
{
    pantheios::log_INFORMATIONAL("GerUserAfterValidation(uid = ", pantheios::integer(uid), ", password = ", password);

    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Users "
        "WHERE uid = %d;"
        , uid);

    DBResult* ret = dbconn->Execute(querystr);
    if (ret->RowsCount() != 1 || (*ret)[0]["Password"] != password) // uid is unique, so only 0 or 1 row.
    {
        pantheios::log_INFORMATIONAL("Authentication failed!");
        dbconn->Free(&ret);
    }
    else
    {
        pantheios::log_INFORMATIONAL("Authentication passed.");
    }
    return DBResultWrap(ret, dbconn);
}

// TODO: maybe database schema object to manage all primary keys and stuff?
const DBResultWrap deliciousDataAdapter::UpdateRows( DBResultWrap rows, const std::string& table, const std::string& primarykey )
{
    size_t rowcount = rows.getResult()->RowsCount();
    // naive implementation, refactor this when we got time! :)

    vector<std::string> colnames;
    vector<std::string> values;
    string query;
    for (size_t i=0;i<rowcount;i++)
    {
        DBRow& row = rows.getResult()->GetRow(i);
        if (row.ColumnModified().size())
        {
            for (size_t j=0;j<row.ColumnModified().size();j++)
            {
                query += "SET " + rows.getResult()->ColumnName(j) + "= '" + row.GetValueAs<string>(j) + "', ";
            }

            query.erase(query.size()-2);

            query = "UPDATE " + table + query + "WHERE " + primarykey + " = " + row.GetValueAs<string>(primarykey);
            dbconn->Execute(query);
        }
    }
    return rows;
}

char* tmToSqliteTimeModifiers(char* buf, tm& time)
{
    char *step = buf;
#define MODIFIER_FOR( tm_part, modifier_name )                                  \
    if (time.tm_##tm_part > 0)                                                   \
        step += sprintf(step, ", '%d %s'", time.tm_##tm_part, #modifier_name);

    MODIFIER_FOR( year, years);
    MODIFIER_FOR( mon, months);
    MODIFIER_FOR( mday, days);
    MODIFIER_FOR( hour, hours);
    MODIFIER_FOR( min, minutes);
    MODIFIER_FOR( sec, seconds);

    return buf;
#undef MODIFIER_FOR
}

size_t deliciousDataAdapter::AddMessagesToDB( int from_uid, int to_uid, const std::string& text, tm validTimePeriod )
{
    pantheios::log_INFORMATIONAL("AddMessagesToDB(",
        "from_uid=", pantheios::integer(from_uid),
        ", to_uid=", pantheios::integer(to_uid),
        ", text=",text,
        ", tm=", validTimePeriod,
        ")");
    char querystr[500], digits[12], timebuf[64], modifierbuf[200];
    sprintf_s(querystr, sizeof(querystr),
        "INSERT INTO Messages "
        "(FromUID, ToUID, AddTime, ExpireTime, MSG) "
        "VALUES (%s, %d, datetime('now'), datetime('now'%s));"
        "SELECT msgid FROM Messages WHERE Messages.rowid = last_insert_rowid();"
        , from_uid == 0 ? "NULL" : itoa(from_uid, digits, 10)
        , to_uid
        , tmToSqliteTimeModifiers(modifierbuf, validTimePeriod));

    DBResult *result = dbconn->Execute(querystr);
    unsigned int msgid = result->GetRow(0).GetValueAs<unsigned int>("MSGID");
    dbconn->Free(&result);
    return msgid;
}

size_t deliciousDataAdapter::RetrieveAllNonDeliveredMessages( CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("RetrieveAllNonDeliveredMessages()");

    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Messages "
        "WHERE ExpireTime > datetime('now') AND Delivered == 0");

    return ExecuteNormal(querystr, callback);
}

void deliciousDataAdapter::ConfirmMessageDelivered( unsigned int msgid )
{
    pantheios::log_INFORMATIONAL("ConfirmMessageDelivered(", "msgid=", pantheios::integer(msgid),")");

    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "UPDATE Messages SET Delivered=1 WHERE msgid=%d;"
        , msgid);

    dbconn->Execute(querystr);
}