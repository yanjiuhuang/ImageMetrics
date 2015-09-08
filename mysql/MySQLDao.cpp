//
// Created by Yanjiu Huang on 8/28/15.
//
#include <iostream>
#include <sstream>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>

#include "MySQLDao.h"


int MySQLDao::init() {

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

MySQLDao::MySQLDao(){
    this->driver = get_driver_instance();
    this->connecton = nullptr;
}

MySQLDao::~MySQLDao() {
    if (this->connecton != nullptr){
        delete this->connecton;
    }
}

template <typename T>
int MySQLDao::query(const DaoQuery& query, QueryResultSet<T>& returnSet) {
    assert(this->connecton != nullptr);

    if(query.hasReturnResult()){
        StmtPtr stmt(this->connecton->createStatement());
        ResultSetPtr resultSet(stmt->executeQuery(query.getContent().c_str()));

        try{
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


        }catch(sql::InvalidArgumentException& e){
            DEBUG_OUTPUT_SQLEXCEPTION(e);
        }


    }

    return EXIT_SUCCESS;
}

unique_ptr<DaoQueryBuilder> DaoQueryBuilder::newBuilder() {
    unique_ptr<DaoQueryBuilder> builderPtr(new DaoQueryBuilder);
    return builderPtr;
}

DaoQueryBuilder::DaoQueryBuilder() {

}

void DaoQueryBuilder::addQueryCondition(const char *key, const int value) {
    this->intTypeConds[key] = value;
}

void DaoQueryBuilder::addQueryCondition(const char *key, const char *value) {
    this->strTypeConds[key] = value;
}

void DaoQueryBuilder::addQueryCondition(const char *key, const vector<string> &valueList) {
    this->strListTypeConds[key] = valueList;
}

void DaoQueryBuilder::addQueryCondition(const char *key, const vector<int> &valueList) {
    this->intListTypeConds[key] = valueList;
}

shared_ptr<DaoQuery> DaoQueryBuilder::build() {
    if(this->qt == QT_NONE){
        return nullptr;
    }

    shared_ptr<DaoQuery> daoQueryPtr(new DaoQuery);
    switch(this->qt){
        case QT_SELECT:
            this->processSelectQuery(daoQueryPtr.get());
            break;
        case QT_INSERT:
            this->processInsertQuery(daoQueryPtr.get());
            break;
        case QT_UPDATE:
            this->processUpdateQuery(daoQueryPtr.get());
            break;
        case QT_DELETE:
            this->processDeleteQuery(daoQueryPtr.get());
            break;
        default:
            return nullptr;
    }

    return daoQueryPtr;
}

void DaoQueryBuilder::setQueryType(QueryType qt) {
    this->qt = qt;
}

void DaoQueryBuilder::processInsertQuery(DaoQuery* const query) {

}

void DaoQueryBuilder::processSelectQuery(DaoQuery* const query) {
    stringstream ss;
    ss << "select ";
    if (this->targetFields.size() <= 0) {
        ss << "* ";
    }
    else{
        ss << boost::algorithm::join(this->targetFields, ", ");
    }
    ss << " from " << this->tableName;

    vector<string> condSmts;
    transform(intTypeConds.begin(), intTypeConds.end(), back_inserter(condSmts), [](IntMapEntryType p){
        return (boost::format("%s=%d") % p.first % p.second).str();
    });
    transform(strTypeConds.begin(), strTypeConds.end(), back_inserter(condSmts), [](StringMapEntryType p){
        return (boost::format("%s='%d'") % p.first % p.second).str();
    });

    transform(intListTypeConds.begin(), intListTypeConds.end(), back_inserter(condSmts), [](IntListMapEntryType p){
        string joins = boost::algorithm::join(p.second | boost::adaptors::transformed(int2String()), ",");
        return (boost::format("%s in (%s)") % p.first % joins).str();
    });

    transform(strListTypeConds.begin(), strListTypeConds.end(), back_inserter(condSmts), [](StringListMapEntryType p){
        return (boost::format("%s in ('%s')") % p.first % boost::algorithm::join(p.second, "','")).str();
    });

    if(condSmts.size() > 0){
        ss << " where " << boost::algorithm::join(condSmts, " and ");
    }

    ss << ";";

    query->setQueryType(this->qt);
    query->setSqlContent(ss.str());
}

void DaoQueryBuilder::processDeleteQuery(DaoQuery* const query) {

}

void DaoQueryBuilder::processUpdateQuery(DaoQuery* const query) {

}

void DaoQueryBuilder::setQueryFields(const vector<string> &fields) {
    this->targetFields.assign(fields.begin(), fields.end());
}

void DaoQuery::setSqlContent(const string& sql) {
    this->sqlContent = sql;
}

void DaoQuery::setQueryType(QueryType qt) {
    this->qt = qt;
}

string DaoQuery::getContent() const {
    return this->sqlContent;
}

QueryType DaoQuery::getQueryType() const {
    return this->qt;
}

string MySQLDao::getDatabaseName() const {
    return "log_statistics";
}

string MySQLDao::getUser() const {
    return "root";
}

string MySQLDao::getPwd() const {
    return "";
}

int MySQLDao::update() {
    return 0;
}

int MySQLDao::insert() {
    return 0;
}

void DaoQueryBuilder::setTable(const char *table) {
    this->tableName = table;
}

bool DaoQuery::hasReturnResult() const {
    return this->qt == QT_SELECT;
}


QueryResultSet::QueryResultSet():resultSet(10){

}

size_t QueryResultSet::size() const {
    return resultSet.size();
}

template <typename T>
void QueryResultSet::addQueryResult(shared_ptr<T> item) {
    this->resultSet.push_back(item);
}

string MySQLDao::getHost() const {
    return "localhost";
}

template <typename T>
typename vector<shared_ptr<T>>::iterator QueryResultSet::begin() {
    return resultSet.begin();
}

template <typename T>
typename vector<shared_ptr<T>>::iterator QueryResultSet::end() {
    return resultSet.end();
}
