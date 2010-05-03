#pragma once
#include <string>

struct sqlite3;
class DBResult;
class DBPrepared;

class DBContext
{
public:

    DBContext(const std::string &connstr);

    void BeginTransaction();

    void Rollback();

    void Commit();

    DBResult* Execute(const std::string &sql);

    DBResult* Execute(DBPrepared*);

    void Free(DBResult**);

    DBPrepared* NewPreparedStatement(const std::string& sql);

    ~DBContext(void);

private:
    static int sqlexeccallback(void * context, int argc, char **argv, char **azColName);
    sqlite3 *db;
/*    bool isInTransaction, isCommit;*/
};
