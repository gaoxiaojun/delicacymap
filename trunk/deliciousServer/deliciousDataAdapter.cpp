#include "StdAfx.h"
#include "deliciousDataAdapter.h"

#include "DBContext.h"
#include "DBResult.h"

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
        Initialize();
    return _single;
}

void deliciousDataAdapter::Initialize()
{
    _single = new deliciousDataAdapter("delicacyDB.db3");
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
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
          "SELECT * \
            FROM Restaurants NATURAL INNER JOIN Relation_Restaurant_RestaurantTypes NATURAL INNER JOIN RestaurantTypes \
            WHERE Longtitude BETWEEN %.10f AND %.10f \
                AND Latitude BETWEEN %.10f AND %.10f"
        , longtitude_from, lontitude_to
        , latitude_from, latitude_to);
    
    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryLatestCommentsOfRestaurant( int rid, int n, CallbackFunc callback )
{
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
          "SELECT Comments.* \
          FROM Relation_Restaurant_Comment NATURAL INNER JOIN Comments \
          WHERE rid=%d \
          ORDER BY addtime \
          LIMIT %d;"
          , rid
          , n);

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryCommentsOfRestaurantSince( int rid, const std::string& timestamp, CallbackFunc callback )
{
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
          "SELECT Comments.* \
          FROM Relation_Restaurant_Comment NATURAL INNER JOIN Comments \
          WHERE rid=%d AND addtime >= '%s';"
          , rid
          , timestamp.c_str());

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryLastestCommentsByUser( int uid, int n, CallbackFunc callback )
{
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
          "SELECT *  \
          FROM Comments \
          WHERE uid=%d \
          ORDER BY addtime \
          LIMIT %d;"
          , uid
          , n);

    ExecuteNormal(querystr, callback);
    return 0;
}

size_t deliciousDataAdapter::QueryCommentsOfUserSince( int uid, const std::string& timestamp, CallbackFunc callback )
{
    char querystr[500];
    sprintf_s(querystr, sizeof(querystr),
          "SELECT * \
          FROM Comments \
          WHERE uid == %d AND addtime >= '%s' \
          ORDER BY addtime;"
          , uid
          , timestamp.c_str());

    ExecuteNormal(querystr, callback);
    return 0;
}
