#pragma once
#include <time.h>
#include <string>
#include <vector>
#include <boost/function.hpp>
#include "MapProtocol.pb.h"

class DBContext;
class DBRow;
class DBResult;
class DBPrepared;

class DBResultWrap
{
public:
	DBResultWrap(DBResult*, DBContext*);
	DBResultWrap(const DBResultWrap&);
	~DBResultWrap();
	bool empty() const;

	DBResult* getResult() const;
private:
	mutable DBResult* result;
	mutable DBContext* context;
};

class deliciousDataAdapter
{
public://callback function type
    typedef boost::function<void (const DBRow&)> CallbackFunc;
protected://constructor
    deliciousDataAdapter(const std::string& connstr);
public://destructor
    ~deliciousDataAdapter(void);

public://singleton impl
    static deliciousDataAdapter* GetInstance();
	static void Initialize(const std::string& connstr);
public://DB methods

    const DBResultWrap UpdateRows( DBResultWrap rows, const std::string& table, const std::string& primarykey );

    /************************************************************************/
    /*                    Method Used by RPCs                               */
    /************************************************************************/

    size_t QueryRestaurantWithinLocation( double longtitude, double latitude, double lontitude_to, double latitude_to, int level, CallbackFunc callback );

    size_t QueryLatestCommentsOfRestaurant( int rid, int n, CallbackFunc callback );

    size_t QueryCommentsOfRestaurantSince( int rid, const std::string& timestamp, CallbackFunc callback );

    size_t QueryLastestCommentsByUser( int uid, int n, CallbackFunc callback );

    size_t QueryCommentsOfUserSince( int uid, const std::string& timestamp, CallbackFunc callback );

    size_t GetRelatedUsersWith( int uid, int relation, CallbackFunc callback );

	const DBResultWrap PostCommentForRestaurant( int rid, int uid, const std::string& msg, const std::string* const image );

	const DBResultWrap UserLogin( const std::string& email, const std::string& password );

    const DBResultWrap GerUserAfterValidation( int uid, const std::string& password );

    /************************************************************************/
    /*                    Methods Used by Messenger                         */
    /************************************************************************/

    size_t AddMessagesToDB( int from_uid, int to_uid, int messageType, const std::string& text, tm validTimePeriod );

    size_t RetrieveAllNonDeliveredMessages( CallbackFunc callback );

    void ConfirmMessageDelivered( unsigned int msgid );

private://methods
    size_t ExecuteNormal( char* query, CallbackFunc callback );
    size_t ExecuteNormal( DBPrepared* query, CallbackFunc callback );
private://data
    static deliciousDataAdapter* _single;

    // prepared statements
    DBPrepared* prepared_Message, *prepared_RestaurantWithinBound, *prepared_ConfirmMessage;
    DBContext *dbconn;
};
