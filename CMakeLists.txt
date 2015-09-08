cmake_minimum_required(VERSION 3.2)
project(ImageMetrics)

include_directories("/opt/local/include")
link_directories("/opt/local/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(ImageMetrics ${SOURCE_FILES} mysql/MySQLDao.cpp mysql/MySQLDao.h mysql/ImgMetricDao.cpp mysql/ImgMetricDao.h)

target_link_libraries(ImageMetrics mysqlcppconn)