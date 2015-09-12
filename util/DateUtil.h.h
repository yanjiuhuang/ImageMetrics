//
// Created by Yanjiu Huang on 9/11/15.
//

#ifndef IMAGEMETRICS_DATEUTIL_H_H
#define IMAGEMETRICS_DATEUTIL_H_H

#include "boost/date_time/gregorian/gregorian.hpp"

#define TODAY boost::gregorian::day_clock::universal_day()

#define TO_SQL_STR(date) boost::gregorian::to_sql_string(date)

#endif //IMAGEMETRICS_DATEUTIL_H_H
