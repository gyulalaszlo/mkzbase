cmake_minimum_required(VERSION 3.6)
project(mkzbase)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


include_directories("${CMAKE_CURRENT_SOURCE_DIR}}/variant/mapbox")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}}/optional")
