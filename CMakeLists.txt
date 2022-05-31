cmake_minimum_required(VERSION 3.22)
project(aeacus)

add_executable(aeacusd src/main.cpp src/main.cpp include/util/argument_parser.h include/util/argument.h include/util/cline_argument_parser.h src/util/cline_argument_parser.cpp)

target_include_directories(aeacusd PUBLIC include)