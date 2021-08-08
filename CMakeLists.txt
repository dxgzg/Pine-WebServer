cmake_minimum_required(VERSION 2.5)
# project(PineNetWork VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
# GDB模式
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(./)
include_directories(./Http)
include_directories(rapidjson)
include (utils.cmake)
# 防止__FILE__警告
add_definitions(-Wno-builtin-macro-redefined)

aux_source_directory(. SRC_LIST1)
aux_source_directory(Http SRC_LIST2)
aux_source_directory(rapidjson SRC_LIST3)

add_executable(Pine ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})
# 加一个动态链接库
target_link_libraries(Pine pthread gflags)
redefine_file_macro(Pine)



