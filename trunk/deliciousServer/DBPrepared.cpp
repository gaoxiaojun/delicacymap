#include "stdafx.h"
#include "DBPrepared.h"

#include "sqlite3.h"

DBPrepared::~DBPrepared( void )
{
    sqlite3_finalize(stmt);
}

void DBPrepared::reset()
{
    sqlite3_reset(stmt);
}

void DBPrepared::bindParameter( int p )
{
    sqlite3_bind_null(stmt, p);
}

void DBPrepared::bindParameter( int p, int data )
{
    sqlite3_bind_int(stmt, p, data);
}

void DBPrepared::bindParameter( int p, unsigned int data )
{
    sqlite3_bind_int64(stmt, p, data);
}

void DBPrepared::bindParameter( int p, long long data )
{
    sqlite3_bind_int64(stmt, p, data);
}

void DBPrepared::bindParameter( int p, double data )
{
    sqlite3_bind_double(stmt, p, data);
}

void DBPrepared::bindParameter( int p, const std::string& data )
{
    sqlite3_bind_text(stmt, p, data.c_str(), data.size(), SQLITE_TRANSIENT);
}

void DBPrepared::bindParameter( int p, const char* data )
{
    sqlite3_bind_text(stmt, p, data, -1, SQLITE_TRANSIENT);
}

void DBPrepared::bindParameter( int p, const char* data, int length )
{
    sqlite3_bind_text(stmt, p, data, length, SQLITE_TRANSIENT);
}

void DBPrepared::bindParameter( int p, const void* data, int length )
{
    sqlite3_bind_blob(stmt, p, data, length, SQLITE_TRANSIENT);
}
