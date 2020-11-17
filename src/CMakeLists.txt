cmake_minimum_required(VERSION 3.11)
project(PineNetWork VERSION 0.1 LANGUAGES CXX)
aux_source_directory(. SRC_LIST)

add_executable(Pine ${SRC_LIST})
target_link_libraries(Pine pthread)