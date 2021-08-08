cmake_minimum_required(VERSION 2.5)
# project(PineNetWork VERSION 0.1 LANGUAGES CXX)


# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

aux_source_directory(. SRC_LIST)

#add_executable(Pine ${SRC_LIST})
# 加一个动态链接库
#target_link_libraries(Pine pthread)



add_library(pine SHARED ${SRC_LIST})

