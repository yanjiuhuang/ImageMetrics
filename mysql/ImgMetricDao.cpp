//
// Created by Yanjiu Huang on 9/8/15.
//

#include "ImgMetricDao.h"

shared_ptr<ImgMetric> ImgMetricDao::createInstance(CursorPtr cursor) {

    string formatString = cursor->getString("date");
    flow_t flowAmount = cursor->getInt64("flow_amount");
    req_size_t reqCount = cursor->getInt64("req_count");

    return shared_ptr<ImgMetric>(new ImgMetric(formatString, flowAmount, reqCount));
}

ImgMetric::ImgMetric():recordDate(INVALID_DATE),metricFlowAmount(0),reqCount(0) {}

ImgMetric::ImgMetric(string formatDate, flow_t &flow, req_size_t &reqCount)
    :metricFlowAmount(0),reqCount(0) {
    this->recordDate = DATE_FROM_STRING(formatDate);
}

ostream &ImgMetric::operator<<(ostream &os, const ImgMetric &dt) {
    os << "Date=" << dt.getDate();
    os << "RequestCount=" << dt.getRequestCount();
    os << "FlowAmount=" << dt.getFlowAmount();
    return os;
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
