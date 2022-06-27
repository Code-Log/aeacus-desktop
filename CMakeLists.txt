cmake_minimum_required(VERSION 3.22)
project(aeacus)
SET(requiredlibs)

add_executable(aeacusd src/main.cpp src/main.cpp include/util/argument_parser.h include/util/argument.h include/util/cline_argument_parser.h src/util/cline_argument_parser.cpp include/event/message_event.h include/user.h src/user.cpp include/reference.h include/util/http_request.h src/util/http_request.cpp include/event/listener.h include/api/message.h include/util/base64.h src/util/base64.cpp src/api/message.cpp include/api/request.h src/api/request.cpp include/util/timer.h src/util/timer.cpp src/daemon.cpp include/daemon.h include/event/command_event.h src/event/command_event.cpp include/api/command.h src/api/command.cpp src/event/listener.cpp src/event/message_event.cpp include/crypto/crypto.h src/crypto/crypto.cpp include/api/token.h src/util/argument.cpp src/util/cline_argument_parser.cpp src/util/InvalidArgumentException.cpp)

find_package(CURL)
if(CURL_FOUND)
    INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
    SET(requiredlibs ${requiredlibs} ${CURL_LIBRARIES} )
else(CURL_FOUND)
    MESSAGE(FATAL_ERROR "Could not find the CURL library and development files.")
endif(CURL_FOUND)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(AEACUS_DEBUG=true)
else ()
    add_compile_definitions(AEACUS_DEBUG=false)
endif ()

find_package(OpenSSL REQUIRED)
find_package(glog REQUIRED)

target_include_directories(aeacusd PUBLIC include)
target_link_libraries(aeacusd curl)
target_link_libraries(aeacusd ${OPENSSL_LIBRARIES})
target_link_libraries(aeacusd sodium)
target_link_libraries(aeacusd glog::glog)