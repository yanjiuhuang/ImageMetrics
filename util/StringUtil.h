//
// Created by Yanjiu Huang on 9/10/15.
//

#ifndef IMAGEMETRICS_STRINGUTIL_H
#define IMAGEMETRICS_STRINGUTIL_H

#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

#define TOSTRING(l)  boost::lexical_cast<std::string>(l)
#define WRAP_STR_VALUE(e) (boost::format("'%s'") % TOSTRING(e)).str()

#define INTS2STRINGS(valueList) (valueList | boost::adaptors::transformed( static_cast<std::string(*)(int)>(std::to_string)))


#define KEYS_OF_MAP(map,keys,EntryType)         transform(map.begin(), map.end(), back_inserter(keys), [](EntryType entry){ return entry.first; });
#define VALUES_OF_MAP(map,values,EntryType)     transform(map.begin(), map.end(), back_inserter(values), [](EntryType entry){ return entry.second; });

#define LIST_JOIN(list,delimeter) boost::algorithm::join(list, delimeter);



#endif //IMAGEMETRICS_STRINGUTIL_H
