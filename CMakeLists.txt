# cmake_minimum_required(VERSION 2.6)
# project(PineNetWork VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
# GDB模式
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(.)
include_directories(rapidjson)
include_directories(./Http)
include_directories(./net/include)
include (utils.cmake)
# 防止__FILE__警告
add_definitions(-Wno-builtin-macro-redefined)



aux_source_directory(. Main)
aux_source_directory(rapidjson Rapidjson)
aux_source_directory(Http Http)
aux_source_directory(net/source NetSource)

add_executable(Pine ${Main} ${Rapidjson} ${Http} ${NetSource})
#add_executable(Pine ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} Http/Header.h Http/HttpCallback.cpp Http/HttpCallback.h Http/Header.cpp)
# 加一个动态链接库
target_link_libraries(Pine pthread gflags)
# sylar的cmake解决FILE不是相对路径的问题
redefine_file_macro(Pine)



