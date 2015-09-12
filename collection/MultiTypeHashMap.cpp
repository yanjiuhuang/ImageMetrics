//
// Created by Yanjiu Huang on 9/10/15.
//

#include <iostream>
#include <stdexcept>

#include "MultiTypeHashMap.h"

MultiTypeHashMap::MultiTypeHashMap() {}

void MultiTypeHashMap::put(const string key, const MultiTypeHashMapValue* value) {
    this->source[key] = value->clone();
}

void MultiTypeHashMap::put(const string key, MultiTypeHashMapValue &value) {
    this->source[key] = value.clone();
}

void MultiTypeHashMap::put(const string key, int value) {
    this->source[key] = IntHashMapValue(value).clone();
}

void MultiTypeHashMap::put(const string key, float value) {
    this->source[key] = FloatHashMapValue(value).clone();
}

void MultiTypeHashMap::put(const string key, const string value) {
    this->source[key] = StringHashMapValue(value).clone();
}

void MultiTypeHashMap::put(const string key, int64_t value) {
    this->source[key] = Int64HashMapValue(value).clone();
}

void MultiTypeHashMap::put(const string key, double value) {
    this->source[key] = DoubleHashMapValue(value).clone();
}

MapValuePtrType MultiTypeHashMap::get(const string key){
    try{
        return this->source[key];
    }catch(out_of_range& e){
        cout << "The exception: " << e.what() << endl;
    }
    return nullptr;
}


