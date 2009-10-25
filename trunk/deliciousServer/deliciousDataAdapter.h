#pragma once
#include <string>
#include <vector>
#include <boost/function.hpp>
#include "MapProtocol.pb.h"

class DBContext;
class DBRow;
class DBResult;

class DBResultWrap
{
public:
	DBResultWrap(DBResult*, DBContext*);
	DBResultWrap(const DBResultWrap&);
	~DBResultWrap();

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
public://query methods
    size_t QueryRestaurantWithinLocation( double longtitude, double latitude, double lontitude_to, double latitude_to, int level, CallbackFunc callback );

    size_t QueryLatestCommentsOfRestaurant( int rid, int n, CallbackFunc callback );

    size_t QueryCommentsOfRestaurantSince( int rid, const std::string& timestamp, CallbackFunc callback );

    size_t QueryLastestCommentsByUser( int uid, int n, CallbackFunc callback );

    size_t QueryCommentsOfUserSince( int uid, const std::string& timestamp, CallbackFunc callback );

	const DBResultWrap PostCommentForRestaurant( int rid, int uid, const std::string& msg, const std::string* const image );

private://methods
    void ExecuteNormal( char* query, CallbackFunc callback );
private://data
    static deliciousDataAdapter* _single;
    DBContext *dbconn;
};
