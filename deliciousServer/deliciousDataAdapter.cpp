#include "stdafx.h"
#include "deliciousDataAdapter.h"

#include "DBContext.h"
#include "DBResult.h"
#include "DBPrepared.h"

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
        prepared_GetUserByUID = NULL;
        prepared_Message = NULL;
        prepared_RestaurantWithinBound = NULL;
        prepared_ConfirmMessage = NULL;
        prepared_GetCommentsOfRest_N = NULL;
        prepared_Login = NULL;
        prepared_InsertComment = NULL;
        dbconn = new DBContext(connstr); 
        DBResult *ret = dbconn->Execute("PRAGMA foreign_keys = true");
        dbconn->Free(&ret);
        prepared_Message = dbconn->NewPreparedStatement(
            "INSERT INTO Messages "
            "(FromUID, ToUID, AddTime, ExpireTime, MessageType, MSG) "
            "VALUES(?, ?, datetime('now'), datetime('now', '5 minutes'), ?, ?);");
        prepared_RestaurantWithinBound = dbconn->NewPreparedStatement(
            "SELECT Restaurants.*, count(cid) as CommentCount, RestaurantTypes.* "
            "FROM Restaurants NATURAL INNER JOIN Relation_Restaurant_RestaurantType NATURAL INNER JOIN RestaurantTypes LEFT OUTER JOIN comments ON Restaurants.rid = Comments.rid "
            "WHERE Longtitude BETWEEN ? AND ? "
            "AND Latitude BETWEEN ? AND ? "
            "GROUP BY rid");
        prepared_ConfirmMessage = dbconn->NewPreparedStatement(
            "UPDATE Messages SET Delivered=1 WHERE msgid=?;");
        prepared_GetUserByUID = dbconn->NewPreparedStatement(
            "SELECT * "
            "FROM Users "
            "WHERE uid = ?;");
        prepared_GetCommentsOfRest_N = dbconn->NewPreparedStatement(
            "SELECT * "
            "FROM Comments INNER JOIN Users ON Users.UID = Comments.UID "
            "WHERE rid=? "
            "ORDER BY addtime "
            "LIMIT ?;");
        prepared_Login = dbconn->NewPreparedStatement(
            "SELECT * "
            "FROM Users "
            "WHERE EmailAddress=? AND Password=?;");
        prepared_InsertComment = dbconn->NewPreparedStatement(
            "INSERT INTO Comments (UID, RID, DID, Comment, PhotoPath, AddTime, TimeZone) VALUES(?, ?, ?, ?, ?, datetime('now'), 8);");
    }
    catch (exception&)
    {
        delete dbconn;
        delete prepared_GetCommentsOfRest_N;
        delete prepared_Login;
        delete prepared_GetUserByUID;
        delete prepared_Message;
        delete prepared_RestaurantWithinBound;
        delete prepared_ConfirmMessage;
        delete prepared_InsertComment;
        throw;
    }
}

deliciousDataAdapter::~deliciousDataAdapter(void)
{
    delete dbconn;
    delete prepared_GetCommentsOfRest_N;
    delete prepared_Login;
    delete prepared_GetUserByUID;
    delete prepared_Message;
    delete prepared_RestaurantWithinBound;
    delete prepared_ConfirmMessage;
    delete prepared_InsertComment;
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
    size_t rows = 0;
    DBResult* ret = dbconn->Execute(query);

    if (ret)
    {
        rows = ret->RowsCount();

        for (size_t i=0;i<rows;++i)
            callback(ret->GetRow(i));

        dbconn->Free(&ret);
    }

    return rows;
}

size_t deliciousDataAdapter::ExecuteNormal( DBPrepared* query, CallbackFunc callback )
{
    size_t rows = 0;
    DBResult* ret = dbconn->Execute(query);

    if (ret)
    {
        rows = ret->RowsCount();

        for (size_t i=0;i<rows;++i)
            callback(ret->GetRow(i));

        dbconn->Free(&ret);
    }

    return rows;
}

size_t deliciousDataAdapter::QueryRestaurantWithinLocation( double longtitude_from, double latitude_from, double longtitude_to, double latitude_to, int level, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryRestaurantWithinLocation(",
        "longtitude_from=", pantheios::real(longtitude_from),
        ",latitude_from=", pantheios::real(latitude_from),
        ",longtitue_to=", pantheios::real(longtitude_to),
        ",latitude_to=", pantheios::real(latitude_to),
        ",level=", pantheios::integer(level),
        ")");

    prepared_RestaurantWithinBound->reset();
    prepared_RestaurantWithinBound->bindParameter(1, longtitude_from);
    prepared_RestaurantWithinBound->bindParameter(2, longtitude_to);
    prepared_RestaurantWithinBound->bindParameter(3, latitude_from);
    prepared_RestaurantWithinBound->bindParameter(4, latitude_to);
    
    return ExecuteNormal(prepared_RestaurantWithinBound, callback);
}

size_t deliciousDataAdapter::QueryLatestCommentsOfRestaurant( int rid, int n, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryLatestCommentsOfRestaurant(",
        "rid=", pantheios::integer(rid),
        ",n=", pantheios::integer(n),
        ")");
    prepared_GetCommentsOfRest_N->reset();
    prepared_GetCommentsOfRest_N->bindParameter(1, rid);
    prepared_GetCommentsOfRest_N->bindParameter(2, n);

    return ExecuteNormal(prepared_GetCommentsOfRest_N, callback);
}

size_t deliciousDataAdapter::QueryCommentsOfRestaurantSince( int rid, const std::string& timestamp, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("QueryCommentsOfRestaurantSince(",
        "rid=", pantheios::integer(rid),
        ",timestamp='", timestamp,
        "')");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * "
        "FROM Comments INNER JOIN Users ON Users.UID = Comments.UID "
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
        "FROM Comments INNER JOIN Users ON Users.UID = Comments.UID "
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
        "FROM Comments INNER JOIN Users ON Users.UID = Comments.UID "
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
        ",msg='", msg,
        "')");
    // Validate user Input!!!!!
    // user input, query might be very long.
    char querystr[500];
    prepared_InsertComment->reset();
    prepared_InsertComment->bindParameter(1, uid);
    prepared_InsertComment->bindParameter(2, rid);
    prepared_InsertComment->bindParameter(3);
    prepared_InsertComment->bindParameter(4, msg);
    if (!image)
        prepared_InsertComment->bindParameter(5);
    else
        prepared_InsertComment->bindParameter(5, *image);
    dbconn->Execute(prepared_InsertComment);
    sprintf_s(querystr, sizeof(querystr),
        "SELECT * FROM Comments WHERE Comments.rowid = last_insert_rowid();");
    return DBResultWrap(dbconn->Execute(querystr), dbconn);
}

const DBResultWrap deliciousDataAdapter::UserLogin( const std::string& email, const std::string& password )
{
    pantheios::log_INFORMATIONAL("UserLogin(",
        "emailAddress=", email,
        ",password=", password,
        ")");
    prepared_Login->reset();
    prepared_Login->bindParameter(1, email);
    prepared_Login->bindParameter(2, password);

    DBResult* ret = dbconn->Execute(prepared_Login);
    if (ret->RowsCount() != 1) // Email is unique, so only 0 or 1 row.
    {
        dbconn->Free(&ret);
    }
    return DBResultWrap(ret, dbconn);
}

const DBResultWrap deliciousDataAdapter::GerUserAfterValidation( int uid, const std::string& password )
{
    pantheios::log_INFORMATIONAL("GerUserAfterValidation(uid = ", pantheios::integer(uid), ", password = ", password);

    prepared_GetUserByUID->reset();
    prepared_GetUserByUID->bindParameter(1, uid);

    DBResult* ret = dbconn->Execute(prepared_GetUserByUID);
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

const DBResultWrap deliciousDataAdapter::GetUserInfo( int uid )
{
    pantheios::log_INFORMATIONAL("GerUserInfo(uid = ", pantheios::integer(uid), ")");

    prepared_GetUserByUID->reset();
    prepared_GetUserByUID->bindParameter(1, uid);

    return DBResultWrap(dbconn->Execute(prepared_GetUserByUID), dbconn);
}


size_t deliciousDataAdapter::GetRelatedUsersWith( int uid, int relation, CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("GetRelatedUsersWith(",
        "uid=", pantheios::integer(uid),
        ",relation=", pantheios::integer(relation),
        ")");
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
        "SELECT Users.* "
        "FROM Relation_User_User INNER JOIN Users ON UID_Target = UID "
        "WHERE UID_Host=%d AND Relation=%d"
        , uid
        , relation);

    return ExecuteNormal(querystr, callback);
}

size_t deliciousDataAdapter::SetUserRelation( int uid, int uid_target, int relation )
{
    size_t retval = 0;
    try
    {
        pantheios::log_INFORMATIONAL("SetUserRelation(",
            "uid=", pantheios::integer(uid),
            ",uid_target=", pantheios::integer(uid_target),
            ",relation=", pantheios::integer(relation),
            ")");
        char querystr[500];
        sprintf_s(querystr, sizeof(querystr),
            "INSERT OR REPLACE INTO Relation_User_User "
            "(UID_Host, UID_Target, Relation) "
            "VALUES(%d, %d, %d);"
            , uid
            , uid_target
            , relation);
        DBResult* ret = dbconn->Execute(querystr);
        retval = ret ? 1 : 0;
        dbconn->Free(&ret);
    }
    catch(...)
    {
    }
    return retval;
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

// ============================================== Message part ===================================================

static inline
char* tmToSqliteTimeModifiers(char* buf, size_t size, tm& time)
{
    char *step = buf;
#define MODIFIER_FOR( tm_part, modifier_name )                                  \
    if (time.tm_##tm_part > 0)                                                   \
        step += sprintf_s(step, size - (step-buf), ", '%d %s'", time.tm_##tm_part, #modifier_name);

    MODIFIER_FOR( year, years);
    MODIFIER_FOR( mon, months);
    MODIFIER_FOR( mday, days);
    MODIFIER_FOR( hour, hours);
    MODIFIER_FOR( min, minutes);
    MODIFIER_FOR( sec, seconds);

    return buf;
#undef MODIFIER_FOR
}

size_t deliciousDataAdapter::AddMessagesToDB( int from_uid, int to_uid, int messageType, const std::string& text, tm validTimePeriod )
{
    pantheios::log_INFORMATIONAL("AddMessagesToDB(",
        "from_uid=", pantheios::integer(from_uid),
        ", to_uid=", pantheios::integer(to_uid),
        /*", text=",text,*/
        /*", tm=", validTimePeriod,*/
        ")");
    char querystr[500], modifierbuf[200];
    prepared_Message->reset();
    if (from_uid)
        prepared_Message->bindParameter(1, from_uid);
    else
        prepared_Message->bindParameter(1);
    prepared_Message->bindParameter(2, to_uid);
    // FIXME: cannot bind "datetime('now' '5 minutes') " to prepared statements, it is hardcoded in the query for now.
//     sprintf_s(modifierbuf, sizeof(modifierbuf), "datetime('now'%s)", tmToSqliteTimeModifiers(querystr, sizeof(modifierbuf)/sizeof(modifierbuf[0]), validTimePeriod));
//     prepared_Message->bindParameter(3, modifierbuf);
    prepared_Message->bindParameter(3, messageType);
    prepared_Message->bindParameter(4, text);

    sprintf_s(querystr, sizeof(querystr),
        "SELECT msgid FROM Messages WHERE Messages.rowid = last_insert_rowid();"
        );

    DBResult *result = dbconn->Execute(prepared_Message);
    dbconn->Free(&result);
    result = dbconn->Execute(querystr);
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

    prepared_ConfirmMessage->reset();
    prepared_ConfirmMessage->bindParameter(1, msgid);

    dbconn->Execute(prepared_ConfirmMessage);
}
