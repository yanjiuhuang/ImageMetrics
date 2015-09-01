//
// Created by Yanjiu Huang on 8/28/15.
//

#ifndef IMAGEMETRICS_MYSQLDAO_H
#define IMAGEMETRICS_MYSQLDAO_H

#include <string>
#include <map>
#include <vector>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

enum QueryType {
    QT_NONE,
    QT_INSERT,
    QT_UPDATE,
    QT_SELECT,
    QT_DELETE
};

class DaoQueryBuilder;

class DaoQuery {
friend class DaoQueryBuilder;
public:
    string getContent() const;
    QueryType getQueryType() const;
private:
    void setQueryType(QueryType qt);
    void setSqlContent(const string& sql);

    QueryType qt;
    string sqlContent;
};

class DaoQueryBuilder {
public:
    static unique_ptr<DaoQueryBuilder> newBuilder();

    shared_ptr<DaoQueryBuilder> setQueryFields(const vector<string>& fields);

    shared_ptr<DaoQueryBuilder> addQueryCondition(const char* key, const int value);
    shared_ptr<DaoQueryBuilder> addQueryCondition(const char* key, const char* value);
    shared_ptr<DaoQueryBuilder> addQueryCondition(const char* key, const vector<string>& valueList);
    shared_ptr<DaoQueryBuilder> addQueryCondition(const char* key, const vector<int>& valueList);

    shared_ptr<DaoQueryBuilder> setQueryType(QueryType qt);
    unique_ptr<DaoQuery> build();

private:

    DaoQueryBuilder();

    void processInsertQuery(DaoQuery* const query);
    void processSelectQuery(DaoQuery* const query);
    void processDeleteQuery(DaoQuery* const query);
    void processUpdateQuery(DaoQuery* const query);

    string tableName;
    QueryType qt = QueryType::QT_NONE;

    map<string, int>            intTypeConds;
    map<string, string>         strTypeConds;
    map<string, vector<int>>    intListTypeConds;
    map<string, vector<string>> strListTypeConds;

    vector<string>              targetFields;

    typedef map<string, int>::value_type                IntMapEntryType;
    typedef map<string, string>::value_type             StringMapEntryType;
    typedef map<string, vector<int>>::value_type        IntListMapEntryType;
    typedef map<string, vector<string>>::value_type     StringListMapEntryType;

};

/**
 * include the result for query, insert, update and delete
 */
class QueryResultSet{

};

class MySQLDao {
public:
    MySQLDao();
    ~MySQLDao();

    int init();

    QueryResultSet query(const DaoQuery& query);
    int update();
    int insert();

    virtual string getDatabaseName() const;
    virtual string getUser() const;
    virtual string getPwd() const;

protected:
    string host;
    int port;

    sql::Driver *driver;
    sql::Connection * connecton;
};

struct int2String
{
    typedef string result_type;
    string operator()(int x) const { return to_string(x); }
};


#endif //IMAGEMETRICS_MYSQLDAO_H
