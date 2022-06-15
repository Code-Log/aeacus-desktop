cmake_minimum_required(VERSION 3.22)
project(aeacus)
include(CTest)

add_executable(aeacusd src/main.cpp src/main.cpp include/util/argument_parser.h include/util/argument.h include/util/cline_argument_parser.h src/util/cline_argument_parser.cpp src/util/argument.cpp include/util/InvalidArgumentException.h src/util/InvalidArgumentException.cpp)
add_executable(testAeacus testSrc/parser.cpp src/util/argument.cpp src/util/cline_argument_parser.cpp src/util/InvalidArgumentException.cpp)

target_include_directories(aeacusd PUBLIC include)
target_include_directories(testAeacus PUBLIC include)

add_test(NAME testParser COMMAND testAeacus)