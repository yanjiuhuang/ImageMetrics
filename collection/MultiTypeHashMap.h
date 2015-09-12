//
// Created by Yanjiu Huang on 9/10/15.
//

#ifndef IMAGEMETRICS_MULTITYPEHASHMAP_H
#define IMAGEMETRICS_MULTITYPEHASHMAP_H

#include <string>
#include <boost/unordered_map.hpp>

#include "../util/StringUtil.h"

using namespace std;

class MultiTypeHashMapValue;
typedef shared_ptr<MultiTypeHashMapValue>   MapValuePtrType;

class MultiTypeHashMapValue{
public:
    MultiTypeHashMapValue(){}
    MultiTypeHashMapValue(const MultiTypeHashMapValue& v) {}

    virtual MapValuePtrType clone() const = 0;
    virtual string toString() const { return ""; }
    virtual bool isIteratable() const { return false; }
};

template <typename T>
class GenericMultiTypeHashMapValue : public MultiTypeHashMapValue{
public:
    GenericMultiTypeHashMapValue(const GenericMultiTypeHashMapValue& v):value(v){}
    GenericMultiTypeHashMapValue(const T& t):value(t){}

    virtual string toString() const { return TOSTRING(value); }
    virtual MapValuePtrType clone() const { return MapValuePtrType(new GenericMultiTypeHashMapValue<T>(value)); }

    operator T&() { return value; }
    operator T() const { return value; }

private:
    T value;
};

typedef GenericMultiTypeHashMapValue<int>       IntHashMapValue;
typedef GenericMultiTypeHashMapValue<float>     FloatHashMapValue;
typedef GenericMultiTypeHashMapValue<double>    DoubleHashMapValue;
typedef GenericMultiTypeHashMapValue<int64_t>   Int64HashMapValue;
typedef GenericMultiTypeHashMapValue<string>    StringHashMapValue;

template <typename T>
class ListMultiTypeHashMapValue : public MultiTypeHashMapValue{
public:

    typedef typename vector<T>::iterator                 iterator;
    typedef typename vector<T>::const_iterator           const_iterator;
    typedef typename vector<T>::reverse_iterator         reverse_iterator;
    typedef typename vector<T>::const_reverse_iterator   const_reverse_iterator;

    ListMultiTypeHashMapValue() {};
    //ListMultiTypeHashMapValue(initializer_list<T> initList):source(initList) {};
    ListMultiTypeHashMapValue(const vector<T>& vec):source(vec) {};

    iterator begin() { return source.begin(); }
    iterator end() { return source.end(); }

    const_iterator cbegin() { return source.cbegin(); }
    const_iterator cend() { return source.cend(); }

    reverse_iterator rbegin() { return source.rbegin(); }
    reverse_iterator rend() { return source.rend(); }

    const_reverse_iterator crbegin() { return source.crbegin(); }
    const_reverse_iterator crend() { return source.crend(); }

    size_t size() const { return source.size(); }

    virtual bool isIteratable() const { return true; }

    virtual MapValuePtrType clone() const {

        return MapValuePtrType(new ListMultiTypeHashMapValue<T>(source));
    };

private:
    vector<T> source;
};

typedef ListMultiTypeHashMapValue<int>       IntListHashMapValue;
typedef ListMultiTypeHashMapValue<float>     FloatListHashMapValue;
typedef ListMultiTypeHashMapValue<double>    DoubleListHashMapValue;
typedef ListMultiTypeHashMapValue<int64_t>   Int64ListHashMapValue;
typedef ListMultiTypeHashMapValue<string>    StringListHashMapValue;

/**
 * Can not make sure the thread safety
 *
 */
class MultiTypeHashMap {
public:

    typedef boost::unordered_map<string, MapValuePtrType>::iterator                 iterator;
    typedef boost::unordered_map<string, MapValuePtrType>::const_iterator           const_iterator;

    MultiTypeHashMap();

    void put(const string key, int value);
    void put(const string key, float value);
    void put(const string key, const string value);
    void put(const string key, int64_t value);
    void put(const string key, double value);

    void put(const string key, const MultiTypeHashMapValue* value);
    void put(const string key, MultiTypeHashMapValue& value);

    MapValuePtrType get(const string key);

    size_t size() const { return this->source.size(); }

    iterator begin() { return source.begin(); }
    iterator end() { return source.end(); }
    const_iterator cbegin() { return source.cbegin(); }
    const_iterator cend() { return source.cend(); }

    typedef boost::unordered_map<string, MapValuePtrType>::value_type EntryType;

private:
    boost::unordered_map<string, MapValuePtrType> source;
};


#endif //IMAGEMETRICS_MULTITYPEHASHMAP_H
