//
// Created by Yanjiu Huang on 8/28/15.
//

#ifndef IMAGEMETRICS_MYSQLDAO_H
#define IMAGEMETRICS_MYSQLDAO_H

#include <string>
#include <map>
#include <vector>

#include <boost/unordered_map.hpp>
#include <boost/array.hpp>
#include <boost/assign.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/function_types/result_type.hpp>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/datatype.h>

using namespace std;

#define DEBUG_OUTPUT_SQLEXCEPTION(e)    cout << "# ERR: SQLException in " << __FILE__; \
                                        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl; \
                                        cout << "# ERR: " << e.what(); \
                                        cout << " (MySQL error code: " << e.getErrorCode(); \
                                        cout << ", SQLState: " << e.getSQLState() << ")" << endl;



template<typename F>
struct result_type_wrapper : boost::function_types::result_type<decltype(&F::operator())>
{
};

template<typename F>
struct adaptable_wrapper : F
{
    typedef typename result_type_wrapper<F>::type result_type;
    adaptable_wrapper(F f) : F(f) {}
};

template<typename F>
adaptable_wrapper<F> make_adaptable(F f)
{
    return adaptable_wrapper<F>(f);
}

enum QueryType {
    QT_NONE,
    QT_INSERT,
    QT_UPDATE,
    QT_SELECT,
    QT_DELETE
};

enum DataType{
    LONG,
    INT,
    FLOAT,
    STRING
};

class QueryResultCursor;
class DaoQueryBuilder;

typedef shared_ptr<sql::ResultSet>          ResultSetPtr;
typedef shared_ptr<sql::Statement>          StmtPtr;
typedef shared_ptr<QueryResultCursor>       CursorPtr;

typedef uint32_t                            idx_t;



class DaoQuery {
friend class DaoQueryBuilder;
public:
    string getContent() const;
    QueryType getQueryType() const;
    bool hasReturnResult() const;
private:
    void setQueryType(QueryType qt);
    void setSqlContent(const string& sql);

    QueryType qt;
    string sqlContent;
};

class DaoQueryBuilder {
public:
    static unique_ptr<DaoQueryBuilder> newBuilder();

    void setQueryFields(const vector<string>& fields);
    void setTable(const char* table);

    void addQueryCondition(const char* key, const int value);
    void addQueryCondition(const char* key, const char* value);
    void addQueryCondition(const char* key, const vector<string>& valueList);
    void addQueryCondition(const char* key, const vector<int>& valueList);

    void setQueryType(QueryType qt);
    shared_ptr<DaoQuery> build();

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
template <typename T>
class QueryResultSet {

friend class MySQLDao;
public:

    QueryResultSet();
    size_t size() const;

    typename vector<shared_ptr<T>>::iterator begin();
    typename vector<shared_ptr<T>>::iterator end();

private:
    void addQueryResult(shared_ptr<T> item);

    vector<shared_ptr<T>> resultSet;
};

class QueryResultCursor {
public:

    static CursorPtr makeCursor(ResultSetPtr source){ return CursorPtr(new QueryResultCursor(source)); }

    inline int getInt(idx_t index){ return resultSource->getInt(index); }
    inline int getInt(string name){ return resultSource->getInt(name); }

    inline int64_t getInt64(idx_t index) { return resultSource->getInt64(index); }
    inline int64_t getInt64(string name) { return resultSource->getInt64(name); }

    inline string getString(idx_t index) { return resultSource->getString(index); }
    inline string getString(string name) { return resultSource->getString(name); }

private:
    QueryResultCursor(ResultSetPtr source): resultSource(source) {} // private constructor
    ResultSetPtr resultSource;
};

template <typename T>
class MySQLDao {
public:

    MySQLDao();
    ~MySQLDao();

    int init();

    int query(const DaoQuery& query, QueryResultSet<T>& resultSet);
    int update();
    int insert();


    virtual string getDatabaseName() const;
    virtual string getUser() const;
    virtual string getPwd() const;
    virtual string getHost() const;
    virtual int getPort() const;

protected:
    inline CursorPtr makeCursor(ResultSetPtr source) { return QueryResultCursor::makeCursor(source); }

    virtual shared_ptr<T> createInstance(CursorPtr cursor);

    // These resource should be shared among all instances for the same host/port
    // But we should check whether cpp driver shared the connection ??
    sql::Driver *driver;
    sql::Connection * connecton;
};

struct int2String
{
    typedef string result_type;
    string operator()(int x) const { return to_string(x); }
};


#endif //IMAGEMETRICS_MYSQLDAO_H
