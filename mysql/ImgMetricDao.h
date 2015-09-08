//
// Created by Yanjiu Huang on 9/8/15.
//

#ifndef IMAGEMETRICS_IMGMETRICDAO_H
#define IMAGEMETRICS_IMGMETRICDAO_H

#include "MySQLDao.h"

#include "boost/date_time/gregorian/gregorian.hpp"

#define INVALID_DATE            boost::date_time::not_a_date_time
#define DATE_FROM_STRING(x)     boost::gregorian::from_string(x)

typedef boost::gregorian::date              date_t;
typedef int64_t                             flow_t;
typedef int64_t                             req_size_t;


class ImgMetric{
public:

    ImgMetric();
    ImgMetric(string formatDate, flow_t& flow, req_size_t& reqCount);

    inline date_t getDate() const{ return this->recordDate; }
    inline flow_t getFlowAmount() const { return this->metricFlowAmount; }
    inline req_size_t getRequestCount() const { return this->reqCount; }

    ostream& operator<<(ostream& os, const ImgMetric& dt);

private:
    date_t      recordDate;
    flow_t      metricFlowAmount;
    req_size_t  reqCount;

};

class ImgMetricDao : public MySQLDao<ImgMetric>{
public:

    virtual string getHost() const;
    virtual int getPort() const;
    virtual string getDatabaseName() const;

protected:
    virtual shared_ptr<ImgMetric> createInstance(CursorPtr cursor);

};


#endif //IMAGEMETRICS_IMGMETRICDAO_H
