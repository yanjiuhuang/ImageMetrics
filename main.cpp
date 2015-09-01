#include <iostream>

#include "mysql/MySQLDao.h"

using namespace std;

int main() {
    unique_ptr<DaoQueryBuilder> builder = DaoQueryBuilder::newBuilder();

    unique_ptr<DaoQuery> query = builder->setQueryFields({"f1", "f2"})
            ->addQueryCondition("f1", 1)
            ->addQueryCondition("f2", "abc")
            ->build();
    cout << query->getContent() << endl;

    return 0;
}