#include "stdafx.h"
#include "DBContext.h"
#include "DBResult.h"
#include "DBPrepared.h"
#include "sqlite3.h"

#include <boost/thread/thread.hpp>
#include <exception>

using namespace std;

DBContext::DBContext(const std::string& connstr)
{
    try
    {
        int err = sqlite3_open_v2(connstr.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);
//         isInTransaction = false;
//         isCommit = false;
        if (!db)
            throw DBexception("Error allocating memory for db handle");
        else if (err != SQLITE_OK)
            throw DBexception(sqlite3_errmsg(db), err);
    }
    catch (const exception&)
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
    catch (const exception&)
    {
        return -1;
    }
    return 0;
}


DBResult* DBContext::Execute( const std::string &sql )
{
    DBResult *result = new DBResult();
    char* errmsg;
    int err = SQLITE_OK;
    int retryCount = 20;
    do {
        err = sqlite3_exec(db, sql.c_str(), sqlexeccallback, result, &errmsg);
        if (err == SQLITE_BUSY)
            boost::thread::yield();
    }while (err == SQLITE_BUSY && retryCount-- > 0);
    if (err != SQLITE_OK)
    {
        const DBexception &e = DBexception(errmsg, err);
		// TODO: this leaks!
        //sqlite3_free(errmsg);
        throw e;
    }

    return result;
}

DBResult* DBContext::Execute( DBPrepared* stmt )
{
    DBResult *result = NULL;
    if (stmt && stmt->isValid())
    {
        int returncode = SQLITE_DONE;
        int retryCount = /*(!isInTransaction || isCommit) ? 20 :*/ 20;
        do{
            returncode = sqlite3_step(stmt->stmt);
            if (returncode == SQLITE_BUSY)
                boost::thread::yield();
        }while (returncode == SQLITE_BUSY && retryCount-- > 0);
        if ( returncode == SQLITE_ROW)
        {
            // this is the case where we need to return data
            size_t columnCount = sqlite3_column_count(stmt->stmt);
            result = new DBResult(columnCount);
            for (size_t i=0;i<columnCount;i++)
            {
                result->SetColumnName(i, sqlite3_column_name(stmt->stmt, i));
            }
            do 
            {
                DBRow& newrow = result->AddRow();
                for (size_t i=0;i<columnCount;i++)
                {
                    const char* col = (const char*)sqlite3_column_text(stmt->stmt, i);
                    newrow[i] = string( col ? col : "" );
                }
                newrow.ResetState();
            } while (sqlite3_step(stmt->stmt) == SQLITE_ROW);
        }
        else if (returncode == SQLITE_DONE)
        {
            // success.
        }
        else
        {
            pantheios::log_WARNING("Execute prepared statement failed, ", sqlite3_errmsg(db), ". error code: ", pantheios::integer(returncode));
            const DBexception &e = DBexception(sqlite3_errmsg(db), returncode);
            throw e;
        }
    }
    return result;
}

DBPrepared* DBContext::NewPreparedStatement( const std::string& sql )
{
    sqlite3_stmt* stmt;
    int err = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);
    if (err != SQLITE_OK)
    {
        pantheios::log_ERROR("Create prepared statement failed, error code: ", pantheios::integer(err));
    }
    return new DBPrepared(stmt);
}

void DBContext::BeginTransaction()
{
    static std::string beginTransaction("BEGIN TRANSACTION;");
    /*isInTransaction = true;*/
    Execute(beginTransaction);
}

void DBContext::Commit()
{
    static std::string endTransaction("END TRANSACTION;");
    /*isCommit = true;*/
    Execute(endTransaction);
//     isCommit = false;
//     isInTransaction = false;
}

void DBContext::Rollback()
{
    static std::string rollback("ROLLBACK TRANSACTION;");
//     isInTransaction = false;
//     isCommit = false;
    Execute(rollback);
}
