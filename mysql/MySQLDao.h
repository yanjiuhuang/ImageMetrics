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
#include <boost/format.hpp>
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

#include "../collection/MultiTypeHashMap.h"

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
    void addQueryCondition(const char* key, const string value);
    void addQueryCondition(const char* key, const vector<string>& valueList);
    void addQueryCondition(const char* key, const vector<int>& valueList);
    void addQueryCondition(const char* key, initializer_list<int> valueList);

    void addValue(const char* key, const int value);
    void addValue(const char* key, const string& value);
    void addValue(const char* key, const double value);
    void addValue(const char* key, const float value);
    void addValue(const char* key, const int64_t value);

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

    vector<string>                              conditionsCnt;
    boost::unordered_map<string, string>        updatesCnt;

    MultiTypeHashMap            conditions;
    MultiTypeHashMap            updates;

    vector<string>              targetFields;

    typedef map<string, int>::value_type                IntMapEntryType;
    typedef boost::unordered_map<string, string>::value_type             StringMapEntryType;
    typedef map<string, vector<int>>::value_type        IntListMapEntryType;
    typedef map<string, vector<string>>::value_type     StringListMapEntryType;

};

/**
 * include the result for query, insert, update and delete
 */
template <typename T>
class QueryResultSet {
public:

    typedef typename vector<shared_ptr<T>>::iterator iterator;
    typedef typename vector<shared_ptr<T>>::const_iterator const_iterator;
    typedef typename vector<shared_ptr<T>>::reverse_iterator reverse_iterator;
    typedef typename vector<shared_ptr<T>>::const_reverse_iterator const_reverse_iterator;

    QueryResultSet();
    size_t size() const;

    iterator begin() { return this->resultSet.begin(); }
    iterator end() { return this->resultSet.end(); }

    reverse_iterator rbegin() { return this->resultSet.rbegin(); }
    reverse_iterator rend() { return this->resultSet.rend(); }

    void addQueryResult(shared_ptr<T> item);

private:
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

    int init();

    int query(const DaoQuery& query, QueryResultSet<T>& resultSet);
    int update();
    int insert();

    virtual void saveInstance(T& t) {/* Do Nothing */};

    virtual string getDatabaseName() const;
    virtual string getUser() const;
    virtual string getPwd() const;
    virtual string getHost() const;
    virtual int getPort() const;

protected:
    MySQLDao();
    ~MySQLDao();

    inline CursorPtr makeCursor(ResultSetPtr source) { return QueryResultCursor::makeCursor(source); }

    virtual shared_ptr<T> createInstance(CursorPtr cursor){ return shared_ptr<T>(new T); }


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


////////////////// The templdate implemenation //////////////

///////////////// The MySQLDao Template ////////////////////

template <typename T>
MySQLDao<T>::MySQLDao(){
    this->driver = get_driver_instance();
    this->connecton = nullptr;
}

template <typename T>
MySQLDao<T>::~MySQLDao() {
    if (this->connecton != nullptr){
        delete this->connecton;
    }
}

template <typename T>
int MySQLDao<T>::init() {

    cout << "Connector/C++ Driver Initialization ..." << endl;
    cout << endl;

    try {

        stringstream ss;
        ss << boost::format("tcp://%s:%d") % this->getHost() % this->getPort();
        string targetHost;
        ss >> targetHost;

        cout << "Try to connect the MySQL server: " << targetHost << endl;

        this->connecton = this->driver->connect(targetHost, this->getUser(), this->getPwd());
        StmtPtr stmtPtr(this->connecton->createStatement());
        stmtPtr->execute("USE " + this->getDatabaseName());

    } catch (sql::SQLException &e) {
        DEBUG_OUTPUT_SQLEXCEPTION(e);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

template <typename T>
string MySQLDao<T>::getHost() const {
    return "localhost";
}


template <typename T>
int MySQLDao<T>::getPort() const {
    return 3306;
}

template <typename T>
string MySQLDao<T>::getDatabaseName() const {
    return "log_statistics";
}

template <typename T>
string MySQLDao<T>::getUser() const {
    return "root";
}

template <typename T>
string MySQLDao<T>::getPwd() const {
    return "";
}

template <typename T>
int MySQLDao<T>::update() {
    // TODO
    return 0;
}

template <typename T>
int MySQLDao<T>::insert() {
    return 0;
}

template <typename T>
int MySQLDao<T>::query(const DaoQuery& query, QueryResultSet<T>& returnSet) {
    assert(this->connecton != nullptr);
    cout << "Start the query: " << query.getContent() << endl;
    try{
        StmtPtr stmt(this->connecton->createStatement());
        sql::SQLString sql = query.getContent();
        ResultSetPtr resultSet(stmt->executeQuery(sql));

        if(query.hasReturnResult()){

            sql::ResultSetMetaData* metaData = resultSet->getMetaData();
            unsigned int columnCount = metaData->getColumnCount();
            for(unsigned int i = 1; i <= columnCount; i++){
                int type = metaData->getColumnType(i);
                string typeName = metaData->getColumnTypeName(i);

                cout << "Column [" << i << "] type=" << type << ", name=" << typeName << endl;
            }

            while(resultSet->next()){
                shared_ptr<T> result = createInstance(makeCursor(resultSet));
                returnSet.addQueryResult(result);
            }
        }
        cout << "Complete the query: " << query.getContent() << endl;
    }catch(sql::SQLException& e){
        if (e.getErrorCode() != 0){
            DEBUG_OUTPUT_SQLEXCEPTION(e);
        }
    }

    return EXIT_SUCCESS;
}

////// The QueryResultSet Template /////////////////////////
template <typename T>
QueryResultSet<T>::QueryResultSet():resultSet(){

}

template <typename T>
size_t QueryResultSet<T>::size() const {
    return resultSet.size();
}

template <typename T>
void QueryResultSet<T>::addQueryResult(shared_ptr<T> item) {
    this->resultSet.push_back(item);
}

//////////////// End Of The Template implementation /////////////////


#endif //IMAGEMETRICS_MYSQLDAO_H
