#include "StdAfx.h"
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
    }
    catch (exception&)
    {
        delete dbconn;
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
          "SELECT * "
          "FROM Restaurants NATURAL INNER JOIN Relation_Restaurant_RestaurantType NATURAL INNER JOIN RestaurantTypes "
          "WHERE Longtitude BETWEEN %.10f AND %.10f "
                "AND Latitude BETWEEN %.10f AND %.10f"
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
          "FROM Relation_Restaurant_Comment NATURAL INNER JOIN Comments "
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
		"INSERT INTO Comments (UID, Comment) VALUES(%d, \"%s\");"
		"INSERT INTO Relation_Restaurant_Comment (rid, cid) values (%d, (select last_insert_rowid()));"
		"SELECT * from Comments NATURAL INNER JOIN Relation_Restaurant_Comment WHERE Relation_Restaurant_Comment.rowid = (select last_insert_rowid());"
		, uid
		, msg.c_str()
		, rid);
	return DBResultWrap(dbconn->Execute(querystr), dbconn);
}
