#pragma once
#include <string>

struct sqlite3;
class DBResult;

class DBContext
{
public:

    DBContext(const std::string &connstr);

    DBResult* Execute(const std::string &sql);

    void Free(DBResult**);

    ~DBContext(void);

private:
    static int sqlexeccallback(void * context, int argc, char **argv, char **azColName);
    sqlite3 *db;
};
