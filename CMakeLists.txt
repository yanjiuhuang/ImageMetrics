cmake_minimum_required(VERSION 3.2)
project(ImageMetrics)

include_directories("/opt/local/include")
link_directories("/opt/local/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_COMPILER "g++")

add_library(MySQLDao mysql/MySQLDao.cpp mysql/MySQLDao.h mysql/ImgMetricDao.cpp mysql/ImgMetricDao.h)
target_link_libraries(MySQLDao mysqlcppconn boost_chrono-mt boost_date_time-mt boost_system-mt)

set(SOURCE_FILES main.cpp collection/MultiTypeHashMap.cpp collection/MultiTypeHashMap.h util/StringUtil.h util/DateUtil.h.h)
add_executable(ImageMetrics ${SOURCE_FILES})


target_link_libraries(ImageMetrics MySQLDao)