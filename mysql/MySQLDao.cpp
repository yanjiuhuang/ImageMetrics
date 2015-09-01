//
// Created by Yanjiu Huang on 8/28/15.
//
#include <iostream>
#include <sstream>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include "MySQLDao.h"


int MySQLDao::init() {

    cout << "Connector/C++ Driver Initialization ..." << endl;
    cout << endl;

    try {
        stringstream ss;
        ss << boost::format("tcp://%s:%d") % this->host % this->port;
        string targetHost;
        ss >> targetHost;

        this->connecton = this->driver->connect(targetHost, this->getUser(), this->getPwd());
    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

MySQLDao::MySQLDao(): host("localhost"),
                      port(3306){
    this->driver = get_driver_instance();
    this->connecton = nullptr;
}

MySQLDao::~MySQLDao() {
    if (this->connecton != nullptr){
        delete(this->connecton);
    }
}


QueryResultSet MySQLDao::query(const DaoQuery& query) {

}

unique_ptr<DaoQueryBuilder> DaoQueryBuilder::newBuilder() {
    unique_ptr<DaoQueryBuilder> builderPtr(new DaoQueryBuilder);
    return builderPtr;
}

DaoQueryBuilder::DaoQueryBuilder() {

}

shared_ptr<DaoQueryBuilder> DaoQueryBuilder::addQueryCondition(const char *key, const int value) {
    this->intTypeConds[key] = value;
    return shared_ptr<DaoQueryBuilder>(this);
}

shared_ptr<DaoQueryBuilder> DaoQueryBuilder::addQueryCondition(const char *key, const char *value) {
    this->strTypeConds[key] = value;
    return shared_ptr<DaoQueryBuilder>(this);
}

shared_ptr<DaoQueryBuilder> DaoQueryBuilder::addQueryCondition(const char *key, const vector<string> &valueList) {
    this->strListTypeConds[key] = valueList;
    return shared_ptr<DaoQueryBuilder>(this);
}

shared_ptr<DaoQueryBuilder> DaoQueryBuilder::addQueryCondition(const char *key, const vector<int> &valueList) {
    this->intListTypeConds[key] = valueList;
    return shared_ptr<DaoQueryBuilder>(this);
}

unique_ptr<DaoQuery> DaoQueryBuilder::build() {
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

    return unique_ptr<DaoQuery>(daoQueryPtr.get());
}

shared_ptr<DaoQueryBuilder> DaoQueryBuilder::setQueryType(QueryType qt) {
    this->qt = qt;
    return shared_ptr<DaoQueryBuilder>(this);
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
        ss << boost::algorithm::join(this->targetFields, " ");
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

    query->setQueryType(this->qt);
    query->setSqlContent(ss.str());
}

void DaoQueryBuilder::processDeleteQuery(DaoQuery* const query) {

}

void DaoQueryBuilder::processUpdateQuery(DaoQuery* const query) {

}

shared_ptr<DaoQueryBuilder> DaoQueryBuilder::setQueryFields(const vector<string> &fields) {
    this->targetFields.assign(fields.begin(), fields.end());
    return shared_ptr<DaoQueryBuilder>(this);
}

void DaoQuery::setSqlContent(const string& sql) {
    this->sqlContent = sql;
}

void DaoQuery::setQueryType(QueryType qt) {
    this->setQueryType(qt);
}

string DaoQuery::getContent() const {
    return this->sqlContent;
}

QueryType DaoQuery::getQueryType() const {
    return this->qt;
}
