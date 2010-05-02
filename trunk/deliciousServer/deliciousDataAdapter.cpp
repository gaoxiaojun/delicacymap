#include "stdafx.h"
#include "deliciousDataAdapter.h"

#include "DBContext.h"
#include "DBResult.h"
#include "DBPrepared.h"

#include <pantheios/inserters/real.hpp>

#include <exception>

using namespace std;
using namespace ProtocolBuffer;

//deliciousDataAdapter* deliciousDataAdapter::_single = NULL;
std::string deliciousDataAdapter::dbpath;
DBContext* deliciousDataAdapter::dbconn;

deliciousDataAdapter::deliciousDataAdapter(const std::string& connstr)
{
    try
    {
        prepared_GetUserByUID = NULL;
        prepared_Message = NULL;
        prepared_RestaurantWithinBound = NULL;
        prepared_ConfirmMessage = NULL;
        prepared_GetCommentsOfRest_N = NULL;
        prepared_Login = NULL;
        prepared_InsertComment = NULL;
        prepared_AddRestaurant = NULL;
        prepared_SearchRestaurants = NULL;
        prepared_SearchUsers = NULL;
        prepared_Subscription = NULL;
        prepared_UpdateUserSubscription = NULL;
        prepared_UpdateRestaurantSubscription = NULL;
        prepared_InsertRelationRestaurantType = NULL;
        prepared_SubscribtionUser = NULL;
        prepared_SubscribtionRestaurant = NULL;
        prepared_GetUserByEmail = NULL;
        prepared_RegisterUser = NULL;
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
        prepared_AddRestaurant = dbconn->NewPreparedStatement(
            "INSERT INTO Restaurants (Name, Latitude, Longtitude, AverageExpense) VALUES(?, ?, ?, ?);");
        prepared_InsertRelationRestaurantType = dbconn->NewPreparedStatement(
            "INSERT INTO Relation_Restaurant_RestaurantType (RID, TID) VALUES (?, ?);");
        prepared_SearchRestaurants = dbconn->NewPreparedStatement(
            "SELECT * "
            "FROM Restaurants NATURAL INNER JOIN Relation_Restaurant_RestaurantType NATURAL INNER JOIN RestaurantTypes "
            "WHERE Restaurants.Name LIKE :1 OR RestaurantTypes.ReadableText LIKE :1;");
        prepared_SearchUsers = dbconn->NewPreparedStatement(
            "SELECT * "
            "FROM Users "
            "WHERE nickname LIKE :1 OR emailAddress LIKE :1;");
        prepared_Subscription = dbconn->NewPreparedStatement(
            "SELECT C.*,U.*,R.* FROM Comments AS C "
            "INNER JOIN Users AS U ON U.UID = C.UID "
            "LEFT OUTER JOIN Relation_User_User AS RU ON C.uid = RU.uid_target "
            "LEFT OUTER JOIN Relation_User_Restaurant AS RR ON C.rid = RR.rid "
            "NATURAL INNER JOIN Restaurants as R "
            "WHERE C.addtime > (SELECT SubscriptionCheckTime FROM Users WHERE Users.uid = :1) "
            "AND ((RU.UID_Host = :1 AND RU.Subscription) "
            "OR (RR.UID = :1 AND RR.Subscription));");
        prepared_UpdateUserSubscription = dbconn->NewPreparedStatement(
            "INSERT OR REPLACE INTO Relation_User_User "
            "(UID_Host, UID_Target, Subscription) "
            "VALUES(?, ?, ?);");
        prepared_UpdateRestaurantSubscription = dbconn->NewPreparedStatement(
            "INSERT OR REPLACE INTO Relation_User_Restaurant "
            "(UID, RID, Subscription) "
            "VALUES(?, ?, ?);");
        prepared_SubscribtionUser = dbconn->NewPreparedStatement(
            "SELECT Users.* FROM Users "
            "INNER JOIN Relation_User_User AS RU ON Users.UID = RU.UID_Target "
            "WHERE RU.UID_Host = ? AND RU.Subscription = 1;");
        prepared_SubscribtionRestaurant = dbconn->NewPreparedStatement(
            "SELECT Restaurants.* FROM Restaurants "
            "NATURAL INNER JOIN Relation_User_Restaurant AS RU "
            "WHERE RU.UID = ? AND RU.Subscription = 1;");
        prepared_GetUserByEmail = dbconn->NewPreparedStatement(
            "SELECT * "
            "FROM Users "
            "WHERE EmailAddress = ?;");
        prepared_RegisterUser = dbconn->NewPreparedStatement(
            "INSERT INTO Users "
            "(EmailAddress, Password, Nickname, JoinTime, LastLatitude, LastLongitude, SubscriptionCheckTime) "
            "VALUES (?, ?, ?, datetime('now'), 0, 0, datetime('now'));");
    }
    catch (exception&)
    {
        delete prepared_GetCommentsOfRest_N;
        delete prepared_Login;
        delete prepared_GetUserByUID;
        delete prepared_Message;
        delete prepared_RestaurantWithinBound;
        delete prepared_ConfirmMessage;
        delete prepared_InsertComment;
        delete prepared_AddRestaurant;
        delete prepared_SearchRestaurants;
        delete prepared_SearchUsers;
        delete prepared_Subscription;
        delete prepared_UpdateUserSubscription;
        delete prepared_UpdateRestaurantSubscription;
        delete prepared_SubscribtionUser;
        delete prepared_SubscribtionRestaurant;
        delete prepared_GetUserByEmail;
        delete prepared_RegisterUser;
        throw;
    }
}

deliciousDataAdapter::~deliciousDataAdapter(void)
{
    delete prepared_GetCommentsOfRest_N;
    delete prepared_Login;
    delete prepared_GetUserByUID;
    delete prepared_Message;
    delete prepared_RestaurantWithinBound;
    delete prepared_ConfirmMessage;
    delete prepared_InsertComment;
    delete prepared_AddRestaurant;
    delete prepared_SearchRestaurants;
    delete prepared_SearchUsers;
    delete prepared_Subscription;
    delete prepared_UpdateUserSubscription;
    delete prepared_UpdateRestaurantSubscription;
    delete prepared_SubscribtionUser;
    delete prepared_SubscribtionRestaurant;
    delete prepared_GetUserByEmail;
    delete prepared_RegisterUser;
}

deliciousDataAdapter* deliciousDataAdapter::NewInstance()
{
//     if (!_single)
//         Initialize("delicacyDB.db3"); //TODO: give more rational solution.
//     return _single;
    return new deliciousDataAdapter(dbpath);
}

void deliciousDataAdapter::Initialize(const string& dbfile)
{
    //_single = new deliciousDataAdapter(dbfile);
    dbpath = dbfile;
    dbconn = new DBContext(dbfile); 
    DBResult *ret = dbconn->Execute("PRAGMA foreign_keys = true");
    dbconn->Free(&ret);
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
    prepared_InsertComment->reset();
    prepared_InsertComment->bindParameter(1, uid);
    prepared_InsertComment->bindParameter(2, rid);
    prepared_InsertComment->bindParameter(3);
    prepared_InsertComment->bindParameter(4, msg);
    if (!image)
        prepared_InsertComment->bindParameter(5);
    else
        prepared_InsertComment->bindParameter(5, *image);
    dbconn->BeginTransaction();
    dbconn->Execute(prepared_InsertComment);
    DBResultWrap result( dbconn->Execute("SELECT * FROM Comments NATURAL INNER JOIN Users WHERE Comments.rowid = last_insert_rowid();"), dbconn );
    dbconn->EndTransaction();
    return result;
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

    return DBResultWrap(dbconn->Execute(prepared_Login), dbconn);
}

const DBResultWrap deliciousDataAdapter::GetUserAfterValidation( int uid, const std::string& password )
{
    pantheios::log_INFORMATIONAL("GetUserAfterValidation(uid = ", pantheios::integer(uid), ", password = ", password);

    prepared_GetUserByUID->reset();
    prepared_GetUserByUID->bindParameter(1, uid);

    DBResult* ret = dbconn->Execute(prepared_GetUserByUID);
    if (!ret || ret->RowsCount() != 1 || (*ret)[0]["Password"] != password) // uid is unique, so only 0 or 1 row.
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

const DBResultWrap deliciousDataAdapter::GetUserInfo( const std::string& email )
{
    pantheios::log_INFORMATIONAL("GerUserInfo(email='",email, "')");

    prepared_GetUserByEmail->reset();
    prepared_GetUserByEmail->bindParameter(1, email);

    return DBResultWrap(dbconn->Execute(prepared_GetUserByEmail), dbconn);
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
        "WHERE UID_Host = %d AND Relation = %d"
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

const DBResultWrap deliciousDataAdapter::AddRestaurant( const std::string& rname, double latitude, double longitude, int type, int averageExpense )
{
    pantheios::log_INFORMATIONAL("AddRestaurant(",
        "latitude=", pantheios::real(latitude),
        ",longitude=", pantheios::real(longitude),
        ")");
    prepared_AddRestaurant->reset();
    prepared_AddRestaurant->bindParameter(1, rname);
    prepared_AddRestaurant->bindParameter(2, latitude);
    prepared_AddRestaurant->bindParameter(3, longitude);
    prepared_AddRestaurant->bindParameter(4, averageExpense);

    type = type >= 0 ? type : 0;
    type = type < 14 ? type : 0; // hard code this for now
    prepared_InsertRelationRestaurantType->reset();
    prepared_InsertRelationRestaurantType->bindParameter(2, type);

    dbconn->BeginTransaction();
    dbconn->Execute(prepared_AddRestaurant);
    DBResultWrap result(dbconn->Execute("SELECT Restaurants.* FROM Restaurants WHERE Restaurants.rowid = last_insert_rowid();"), dbconn);
    prepared_InsertRelationRestaurantType->bindParameter(1, result.getResult()->GetRow(0).GetValueAs<int>("RID"));
    dbconn->Execute(prepared_InsertRelationRestaurantType);
    dbconn->EndTransaction();

    return result;
}

const DBResultWrap deliciousDataAdapter::SearchRestaurant( const std::string& text )
{
    pantheios::log_INFORMATIONAL("SearchRestaurant(",
        "text=", text,
        ")");
    std::string clause = "%" + text + "%";
    prepared_SearchRestaurants->reset();
    prepared_SearchRestaurants->bindParameter(1, clause);
    return DBResultWrap(dbconn->Execute(prepared_SearchRestaurants), dbconn);
}

const DBResultWrap deliciousDataAdapter::SearchUser( const std::string& text )
{
    pantheios::log_INFORMATIONAL("SearchUser(",
        "text=", text,
        ")");
    std::string clause = "%" + text + "%";
    prepared_SearchUsers->reset();
    prepared_SearchUsers->bindParameter(1, clause);
    return DBResultWrap(dbconn->Execute(prepared_SearchUsers), dbconn);
    return DBResultWrap(NULL, dbconn);
}

const DBResultWrap deliciousDataAdapter::GetSubscribedUserBy( int uid )
{
    pantheios::log_INFORMATIONAL("GetSubscribedUserBy(",
        "uid=", pantheios::integer(uid), ")");

    prepared_SubscribtionUser->reset();
    prepared_SubscribtionUser->bindParameter(1, uid);
    
    return DBResultWrap(dbconn->Execute(prepared_SubscribtionUser), dbconn);
}

const DBResultWrap deliciousDataAdapter::GetSubscribedRestaurantBy( int uid )
{
    pantheios::log_INFORMATIONAL("GetSubscribedRestaurantBy(",
        "uid=", pantheios::integer(uid), ")");

    prepared_SubscribtionRestaurant->reset();
    prepared_SubscribtionRestaurant->bindParameter(1, uid);

    return DBResultWrap(dbconn->Execute(prepared_SubscribtionRestaurant), dbconn);
}

const DBResultWrap deliciousDataAdapter::AddUser( const std::string& nickname, const std::string& email, const std::string& pwd )
{
    pantheios::log_INFORMATIONAL("AddUser(",
        "email='", email, "')");

    prepared_RegisterUser->reset();
    prepared_RegisterUser->bindParameter(1, email);
    prepared_RegisterUser->bindParameter(2, pwd);
    prepared_RegisterUser->bindParameter(3, nickname);

    dbconn->Execute(prepared_RegisterUser);

    return GetUserInfo(email);
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
                size_t modified_Col = row.ColumnModified()[j];
                //hack: special case datetime("now")
                if (row.GetValueAs<string>(modified_Col) == "datetime(\"now\")")
                    query += rows.getResult()->ColumnName(modified_Col) + "=" + row.GetValueAs<string>(modified_Col) + ", ";
                else
                    query += rows.getResult()->ColumnName(modified_Col) + "= '" + row.GetValueAs<string>(modified_Col) + "', ";
            }

            query.erase(query.size()-2);

            query = ("UPDATE " + table + " SET ") + query + (" WHERE " + primarykey + " = " + row.GetValueAs<string>(primarykey));
            dbconn->Execute(query);
        }
    }
    return rows;
}

// ============================================== Message part ===================================================

const DBResultWrap deliciousDataAdapter::GetUserLocations()
{
    return DBResultWrap(NULL, NULL);
}

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
    char modifierbuf[200];
    prepared_Message->reset();
    prepared_Message->bindParameter(1, from_uid);
    prepared_Message->bindParameter(2, to_uid);
    // FIXME: cannot bind "datetime('now' '5 minutes') " to prepared statements, it is hardcoded in the query for now.
//     sprintf_s(modifierbuf, sizeof(modifierbuf), "datetime('now'%s)", tmToSqliteTimeModifiers(querystr, sizeof(modifierbuf)/sizeof(modifierbuf[0]), validTimePeriod));
//     prepared_Message->bindParameter(3, modifierbuf);
    prepared_Message->bindParameter(3, messageType);
    prepared_Message->bindParameter(4, text);

    dbconn->BeginTransaction();
    dbconn->Execute(prepared_Message);
    DBResultWrap result(dbconn->Execute("SELECT msgid FROM Messages WHERE Messages.rowid = last_insert_rowid();"), dbconn);
    dbconn->EndTransaction();

    return result.getResult()->GetRow(0).GetValueAs<unsigned int>("MSGID");
}

const DBResultWrap deliciousDataAdapter::GetSubscriptionForUserSinceLastUpdate( int uid )
{
    pantheios::log_INFORMATIONAL("GetSubscriptionForUserSinceLastUpdate(uid=", pantheios::integer(uid), ")");

    prepared_Subscription->reset();
    prepared_Subscription->bindParameter(1, uid);

    dbconn->BeginTransaction();
    DBResultWrap result(dbconn->Execute(prepared_Subscription), dbconn);

    char update[100];
    sprintf_s(update, sizeof(update), "UPDATE Users SET SubscriptionCheckTime=datetime(\"now\") WHERE UID=%d;", uid);

    dbconn->Execute(update);
    dbconn->EndTransaction();

    return result;
}

void deliciousDataAdapter::ChangeSubsciptionStatusWithUser( int me, int target, bool subscribe )
{
    pantheios::log_INFORMATIONAL("ChangeSubsciptionStatusWithUser(",
        "me=", pantheios::integer(me),
        ",target=", pantheios::integer(target),
        ",subscribe=", pantheios::integer(subscribe),
        ")");
    
    prepared_UpdateUserSubscription->reset();
    prepared_UpdateUserSubscription->bindParameter(1, me);
    prepared_UpdateUserSubscription->bindParameter(2, target);
    prepared_UpdateUserSubscription->bindParameter(3, subscribe);

    dbconn->Execute(prepared_UpdateUserSubscription);

    pantheios::log_INFORMATIONAL("Leave ChangeSubsciptionStatusWithUser()");
}

void deliciousDataAdapter::ChangeSubsciptionStatusWithRestaurant( int me, int target, bool subscribe )
{
    pantheios::log_INFORMATIONAL("ChangeSubsciptionStatusWithRestaurant(",
        "me=", pantheios::integer(me),
        ",target=", pantheios::integer(target),
        ",subscribe=", pantheios::integer(subscribe),
        ")");

    prepared_UpdateRestaurantSubscription->reset();
    prepared_UpdateRestaurantSubscription->bindParameter(1, me);
    prepared_UpdateRestaurantSubscription->bindParameter(2, target);
    prepared_UpdateRestaurantSubscription->bindParameter(3, subscribe);

    dbconn->Execute(prepared_UpdateRestaurantSubscription);

    pantheios::log_INFORMATIONAL("Leave ChangeSubsciptionStatusWithRestaurant()");
}

size_t deliciousDataAdapter::RetrieveAllNonDeliveredMessages( CallbackFunc callback )
{
    pantheios::log_INFORMATIONAL("RetrieveAllNonDeliveredMessages()");

    return ExecuteNormal(
        "SELECT * "
        "FROM Messages "
        "WHERE ExpireTime > datetime('now') AND Delivered == 0;"
        , callback);
}

void deliciousDataAdapter::ConfirmMessageDelivered( unsigned int msgid )
{
    pantheios::log_INFORMATIONAL("ConfirmMessageDelivered(", "msgid=", pantheios::integer(msgid),")");

    prepared_ConfirmMessage->reset();
    prepared_ConfirmMessage->bindParameter(1, msgid);

    dbconn->Execute(prepared_ConfirmMessage);
}
