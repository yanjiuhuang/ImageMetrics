//
// Created by Yanjiu Huang on 9/8/15.
//

#include "ImgMetricDao.h"

#include "../util/DateUtil.h.h"

namespace imgmetrics{

    shared_ptr<ImgMetric> ImgMetricDao::createInstance(CursorPtr cursor) {

        string formatString = cursor->getString("date");
        flow_t flowAmount = cursor->getInt64("flow_amount");
        req_size_t reqCount = cursor->getInt64("req_count");

//        cout << " date=" << formatString << " ";
//        cout << " amount=" << TOSTRING(flowAmount) << " ";
//        cout << " reqCount=" << TOSTRING(reqCount) << " ";
//        cout << endl;

        return shared_ptr<ImgMetric>(new ImgMetric(formatString, flowAmount, reqCount));
    }

    ImgMetric::ImgMetric():recordDate(INVALID_DATE),metricFlowAmount(0),reqCount(0) {}

    ImgMetric::ImgMetric(const string& formatDate, flow_t &flow, req_size_t &reqCount)
            :metricFlowAmount(flow),reqCount(reqCount) {
        this->recordDate = DATE_FROM_STRING(formatDate);
    }

    string ImgMetricDao::getHost() const {
        return "10.101.1.52";
    }

    int ImgMetricDao::getPort() const {
        return 3306;
    }

    string ImgMetricDao::getDatabaseName() const {
        return "log_statistics";
    }

    void ImgMetricDao::saveInstance(ImgMetric &t) {
        cout << "Save the ImgMetric Intance" << endl;
        boost::gregorian::to_simple_string(t.getDate());
        unique_ptr<DaoQueryBuilder> builder = DaoQueryBuilder::newBuilder();

        builder->setQueryType(QT_INSERT);
        builder->setTable("image_flow_metrics_daily");
        builder->addValue("date", TO_SQL_STR(t.getDate()));
        builder->addValue("req_count", t.getRequestCount());
        builder->addValue("flow_amount", t.getFlowAmount());

        QueryResultSet<ImgMetric> resultSet;
        shared_ptr<DaoQuery> query = builder->build();
        this->query(*query, resultSet);

    }


}



