#pragma once

#include <string>

struct sqlite3_stmt;

class DBPrepared
{
    friend class  DBContext;
public:
    DBPrepared(sqlite3_stmt* stmt) { this->stmt = stmt; };
    ~DBPrepared(void);

    bool isValid() const { return stmt != NULL; }
    void reset();

    // parameters starts with 1
    void bindParameter(int);
    void bindParameter(int, int);
    void bindParameter(int, unsigned int);
    void bindParameter(int, long long);
    void bindParameter(int, double);
    void bindParameter(int, const std::string&);
    void bindParameter(int, const char*);
    void bindParameter(int, const char*, int);
    void bindParameter(int, const void*, int);


private:
    sqlite3_stmt* stmt;
};
