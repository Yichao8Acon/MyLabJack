cmake_minimum_required(VERSION 3.26)
project(MyLabJack)

set(CMAKE_CXX_STANDARD 17)
include_directories("${CMAKE_SOURCE_DIR}\\include")
add_library(MyLabJack SHARED MyLabJack.cpp)
target_link_libraries(MyLabJack "${CMAKE_SOURCE_DIR}\\lib\\LabJackUD.lib")