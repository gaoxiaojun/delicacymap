#pragma once

#include <utility>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>

class DBRow
{
public:
    friend class DBResult;
    const std::string& operator[](int index) const;
    const std::string& operator[](const std::string& colname) const;

    template <typename To>
    To GetValueAs(const std::string& colname) const
    {
        const string& ret = operator[](colname);
        return boost::lexical_cast<To>(ret);
    }

    template <typename To>
    To GetValueAs(int index) const
    {
        const string& ret = operator[](index);
        return boost::lexical_cast<To>(ret);
    }

    //void swap(const DBRow& other);
private:
    DBRow(DBResult*);
    std::vector<std::string> values;
    DBResult* result;
};

class DBResult
{
public:
    friend class DBContext;
    friend class DBRow;
    DBResult(void);
    ~DBResult(void);

    const string& ColumnName(size_t index);
    size_t RowsCount();
    size_t ColCount();
    const string& Value(size_t Row, size_t Col);

    const DBRow& operator[](int index);
    const DBRow& GetRow(size_t index);

private:
    size_t ResolveColumnName(const std::string& colname);
    void AppendData(int argc, char** argv, char** colname);
    
    typedef std::map<std::string, size_t> ResolveCachesCont;
    ResolveCachesCont resolvecache;
    std::vector<DBRow> data;
    std::vector<std::string> colnames;
};


