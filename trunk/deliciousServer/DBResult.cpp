#include "stdafx.h"
#include "DBResult.h"
#include <assert.h>

using namespace std;

static string EmptyString;

DBResult::DBResult(void)
{
}

DBResult::DBResult( size_t size )
{
    colnames.resize(size);
}

DBResult::~DBResult(void)
{
}

void DBResult::AppendData( int argc, char** argv, char** colname )
{
    data.reserve( data.size() + argc );
    colnames.reserve( data.size() + argc );
    if (colnames.empty())
    {
        for (int i=0;i<argc;++i)
            colnames.push_back( colname[i] );
        colnames.resize(colnames.size());
    }
    DBRow& newrow = AddRow();
    for (int i=0;i<argc;++i)
        newrow[i] = argv[i] ? argv[i] : "" ;
}

const string& DBResult::ColumnName( size_t index ) const
{
    assert(index < ColCount());
    return colnames[index];
}

size_t DBResult::RowsCount() const
{
    return data.size();
}

size_t DBResult::ColCount() const
{
    return colnames.size();
}

const string& DBResult::Value( size_t Row, size_t Col ) const
{
    assert(Row < RowsCount() && Col < ColCount());
    return data[Row][Col];
}

const DBRow& DBResult::GetRow( size_t index ) const
{
    return data[index];
}

DBRow& DBResult::GetRow( size_t index )
{
    return data[index];
}

const DBRow& DBResult::operator[]( int index ) const
{
    return GetRow(index);
}

DBRow& DBResult::operator[]( int index )
{
    return GetRow(index);
}

size_t DBResult::ResolveColumnName( const std::string& colname )
{
    ResolveCachesCont::iterator it = resolvecache.find(colname);
    if (it == resolvecache.end())
    {
        for (size_t i=0;i<colnames.size();++i)
            if (colnames[i] == colname)
            {
                it = resolvecache.insert(ResolveCachesCont::value_type(colname, i)).first;
                break;
            }
    }
    return it==resolvecache.end() ? -1 :it->second;
}

void DBResult::SetColumnName( size_t index, const std::string& colname )
{
    if (index > colnames.size())
        throw;
    colnames[index] = colname;
}

DBRow& DBResult::AddRow()
{
    data.push_back(DBRow(this));
    return data.back();
}

const std::string& DBRow::operator[]( const std::string& colname ) const
{
    assert(result);
    return operator[](result->ResolveColumnName(colname));
}

const std::string& DBRow::operator[]( int index ) const
{
    if (index == -1)
        return EmptyString;
    return values[index];
}

RowModifier DBRow::operator[]( int index )
{
    if (index < 0 || index > values.size())
        throw DBexception("Index out of band");

    return RowModifier(values[index], index, *this);
}

RowModifier DBRow::operator[]( const std::string& colname )
{
    assert(result);
    return operator[](result->ResolveColumnName(colname));
}

DBRow::DBRow( DBResult* parent )
:result(parent)
{
    values.resize(result->ColCount());
}

void DBRow::ResetState()
{
    colmodified.resize(0);
}
