#pragma once

#include <utility>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>

class DBResult;
class DBRow;

class DBexception : public std::exception
{
public:
    explicit DBexception(const char* s=NULL, int code=0) : errmsg(s), errcode(code) {}
    virtual const char* what() const throw(){return errmsg;};
private:
    const char* errmsg;
    int errcode;
};

class RowModifier
{
public:
    RowModifier(std::string &valuetomodified, int index, DBRow& belongto)
        : value(valuetomodified), row(belongto), indx(index)
    {
    }

    operator const std::string&(){return value;}

    template <typename CompareType>
    bool operator==(const CompareType& other) const
    {
        return value == boost::lexical_cast<std::string>(other);
    }

    template <typename CompareType>
    bool operator!=(const CompareType& other) const
    {
        return value != boost::lexical_cast<std::string>(other);
    }

    template <typename ValueType>
    const std::string& operator=(const ValueType& val)
    {
        std::string newvalue = boost::lexical_cast<std::string>(val);
        if (newvalue != value)
        {
            value = newvalue;
            row.colmodified.push_back(indx);
        }

        return value;
    }
private:
    std::string& value;
    DBRow& row;
    const int indx;
};

class DBRow
{
public:
    friend class DBResult;
    friend class RowModifier;
    const std::string& operator[](int index) const;
    const std::string& operator[](const std::string& colname) const;
    RowModifier operator[](int index);
    RowModifier operator[](const std::string& colname);

    template <typename To>
    To GetValueAs(const std::string& colname) const
    {
        const std::string& ret = operator[](colname);
        return boost::lexical_cast<To>(ret);
    }

    template <typename To>
    To GetValueAs(int index) const
    {
        const std::string& ret = operator[](index);
        return boost::lexical_cast<To>(ret);
    }

    const std::vector<int>& ColumnModified() const { return colmodified; }
    void ResetState();

    //void swap(const DBRow& other);
private:
    DBRow(DBResult*);
    std::vector<std::string> values;
    std::vector<int> colmodified;
    DBResult* result;
};

class DBResult
{
public:
    friend class DBContext;
    friend class DBRow;
    DBResult(void);
    DBResult(size_t);
    ~DBResult(void);

    void SetColumnName(size_t index, const std::string& colname);
    const std::string& ColumnName(size_t index) const;
    size_t RowsCount() const;
    size_t ColCount() const;
    const std::string& Value(size_t Row, size_t Col) const;

    const DBRow& operator[](int index) const;
    const DBRow& GetRow(size_t index) const;

    DBRow& operator[](int index);
    DBRow& GetRow(size_t index);

    DBRow& AddRow();

private:
    size_t ResolveColumnName(const std::string& colname);
    void AppendData(int argc, char** argv, char** colname);
    
    typedef std::map<std::string, size_t> ResolveCachesCont;
    ResolveCachesCont resolvecache;
    std::vector<DBRow> data;
    std::vector<std::string> colnames;
};

