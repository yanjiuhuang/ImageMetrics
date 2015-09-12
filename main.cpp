#include <iostream>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/owner_less.hpp>

#include "mysql/ImgMetricDao.h"
#include "collection/MultiTypeHashMap.h"

using namespace std;
using namespace imgmetrics;

shared_ptr<DaoQuery> create_query(){

    unique_ptr<DaoQueryBuilder> builder = DaoQueryBuilder::newBuilder();

    builder->setQueryType(QT_INSERT);
    builder->setTable("image_flow_metrics_daily");
    builder->addValue("name", 1);
    builder->addValue("age", "2");

    shared_ptr<DaoQuery> query = builder->build();

    if (query == nullptr){
        cout << "Nullptr returned" << endl;
    }
    else{
        cout << query->getContent() << endl;
    }

    return query;
}

void test_select_query(){

    shared_ptr<ImgMetricDao> dao_ptr(new ImgMetricDao());
    dao_ptr->init();
    shared_ptr<DaoQuery> query = create_query();
    QueryResultSet<ImgMetric> resultSet;
    dao_ptr->query(*query, resultSet);

    cout << "The query return size: " << resultSet.size() << endl;

    for(auto it = resultSet.begin(); it != resultSet.end(); ++it){
        cout << *(*it) << endl;
    }
}

void test_insert(){
    shared_ptr<ImgMetricDao> dao_ptr(new ImgMetricDao());
    dao_ptr->init();

    string date("2015-09-11");
    flow_t flow = 2;
    req_size_t size = 2;

    ImgMetric img(date, flow, size);

    dao_ptr->saveInstance(img);
}

void test_multi_type_hash_map(){
    MultiTypeHashMap map;

    map.put("int1", 1);
    map.put("float1", 1.1);
    map.put("str1", "abc");

    cout << "size: " << map.size() << endl;

    for_each(map.begin(), map.end(), [](MultiTypeHashMap::EntryType p){
        cout << "key=" << p.first << " ";
        cout << "value=" << p.second->toString() << endl;
    });
}

int main() {
    test_insert();
    return 0;
}