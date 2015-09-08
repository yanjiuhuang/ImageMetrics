#include <iostream>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/owner_less.hpp>

#include "mysql/ImgMetricDao.h"

using namespace std;

shared_ptr<DaoQuery> create_query(){

    unique_ptr<DaoQueryBuilder> builder = DaoQueryBuilder::newBuilder();

    builder->setQueryType(QT_SELECT);
    builder->setTable("image_flow_metrics_daily");

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

    for_each(resultSet.begin(), resultSet.end(), [](shared_ptr<ImgMetric> d){
        cout << *d << endl;
    });
}


int main() {
    test_select_query();
    return 0;
}