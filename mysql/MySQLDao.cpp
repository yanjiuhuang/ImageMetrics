//
// Created by Yanjiu Huang on 8/28/15.
//
#include <iostream>
#include <sstream>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>

#include "MySQLDao.h"
#include "../util/StringUtil.h"

unique_ptr<DaoQueryBuilder> DaoQueryBuilder::newBuilder() {
    unique_ptr<DaoQueryBuilder> builderPtr(new DaoQueryBuilder);
    return builderPtr;
}

DaoQueryBuilder::DaoQueryBuilder() {}

void DaoQueryBuilder::addQueryCondition(const char *key, const int value) {
    this->conditions.put(key, value);
    static boost::format format = boost::format("%s=%d");
    string cnt = (format % key % value).str();
    this->conditionsCnt.push_back(cnt);
}

void DaoQueryBuilder::addQueryCondition(const char *key, const string value) {
    this->conditions.put(key, value);
    static boost::format format = boost::format("%s='%s'");
    string cnt = (format % key % value).str();
    this->conditionsCnt.push_back(cnt);
}

void DaoQueryBuilder::addQueryCondition(const char *key, const vector<string> &valueList) {

    shared_ptr<StringListHashMapValue> p(new StringListHashMapValue(valueList));
    this->conditions.put(key, p.get());

    vector<string> wrappedList;
    transform(valueList.begin(), valueList.end(), back_inserter(wrappedList), [](string s){
        return WRAP_STR_VALUE(s);
    });

    static boost::format format = boost::format("%s in (%s)");
    string cnt = (format % key % boost::algorithm::join(wrappedList, ",")).str();
    this->conditionsCnt.push_back(cnt);
}

void DaoQueryBuilder::addQueryCondition(const char *key, const vector<int> &valueList) {
    shared_ptr<IntListHashMapValue> p(new IntListHashMapValue(valueList));
    this->conditions.put(key, p.get());

    static boost::format format = boost::format("%s in (%s)");
    string cnt = (format % key % boost::algorithm::join(INTS2STRINGS(valueList), ",")).str();
    this->conditionsCnt.push_back(cnt);
}

void DaoQueryBuilder::addQueryCondition(const char *key, initializer_list<int> valueList) {
    vector<int> vec(valueList);
    DaoQueryBuilder::addQueryCondition(key, vec);
}


void DaoQueryBuilder::addValue(const char *key, const int value) {
    this->updates.put(key, value);
    this->updatesCnt[key] = TOSTRING(value);
}

void DaoQueryBuilder::addValue(const char *key, const string& value) {
    this->updates.put(key, value);
    this->updatesCnt[key] = WRAP_STR_VALUE(value);
}

void DaoQueryBuilder::addValue(const char *key, const double value) {
    this->updates.put(key, value);
    this->updatesCnt[key] = TOSTRING(value);
}

void DaoQueryBuilder::addValue(const char *key, const float value) {
    this->updates.put(key, value);
    this->updatesCnt[key] = TOSTRING(value);
}

void DaoQueryBuilder::addValue(const char *key, const int64_t value) {
    this->updates.put(key, value);
    this->updatesCnt[key] = TOSTRING(value);
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
    stringstream ss;
    ss << "insert into " << this->tableName;

    vector<string> keys;
    vector<string> values;
    KEYS_OF_MAP(this->updatesCnt, keys, StringMapEntryType);
    ss << "(" << LIST_JOIN(keys, ",");
    ss << ") ";
    VALUES_OF_MAP(this->updatesCnt, values, StringMapEntryType);
    ss <<  "values (" << LIST_JOIN(values, ",") ;
    ss << ");";

    query->setQueryType(this->qt);
    query->setSqlContent(ss.str());
}

void DaoQueryBuilder::processSelectQuery(DaoQuery* const query) {
    stringstream ss;
    ss << "select ";
    if (this->targetFields.size() <= 0) {
        ss << "* ";
    }
    else{
        ss << LIST_JOIN(this->targetFields, ", ");
    }
    ss << " from " << this->tableName;

    if(conditionsCnt.size() > 0){
        ss << " where " << LIST_JOIN(conditionsCnt, " and ");
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

void DaoQueryBuilder::setTable(const char *table) {
    this->tableName = table;
}

bool DaoQuery::hasReturnResult() const {
    return this->qt == QT_SELECT;
}



