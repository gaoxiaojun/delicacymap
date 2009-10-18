#include "stdafx.h"
#include "DBContext.h"
#include "DBResult.h"
#include "sqlite3.h"

#include <exception>

using namespace std;

DBContext::DBContext(const std::string& connstr)
{
    try
    {
        int err = sqlite3_open_v2(connstr.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
        if (!db)
            throw exception("Error allocating memory for db handle");
        else if (err != SQLITE_OK)
            throw exception(sqlite3_errmsg(db), err);
    }
    catch (exception&)
    {
        sqlite3_close(db);
        throw;
    }
}

DBContext::~DBContext()
{
    sqlite3_close(db);
}

void DBContext::Free( DBResult** db )
{
    delete *db;
    *db = NULL;
}

int DBContext::sqlexeccallback(void * context, int argc, char **argv, char **azColName)
{
    DBResult* result = reinterpret_cast<DBResult*>(context);
    try
    {
        result->AppendData(argc, argv, azColName);
    }
    catch (exception&)
    {
        return -1;
    }
    return 0;
}


DBResult* DBContext::Execute( const std::string &sql )
{
    DBResult *result = new DBResult();
    char* errmsg;
    int err = sqlite3_exec(db, sql.c_str(), sqlexeccallback, result, &errmsg);
    if (err != SQLITE_OK)
    {
        exception &e = exception(errmsg, err);
        sqlite3_free(errmsg);
        throw e;
    }

    return result;
}