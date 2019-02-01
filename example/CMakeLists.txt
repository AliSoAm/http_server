cmake_minimum_required (VERSION 3.3)
project (example)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-g -std=c++14 -pthread")
find_package (Threads)

add_library(http_server STATIC
            ../src/http.cpp
            ../src/http_request.cpp
            ../src/http_server.cpp
            ../src/tcp_server.cpp
            ../src/utils/base64.cpp)
target_link_libraries (http_server  ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(http_server PUBLIC . ../src ../src/utils)

add_executable (http_server_example
                main.cpp
                file.cpp)
target_link_libraries (http_server_example  ${CMAKE_THREAD_LIBS_INIT} http_server)
target_include_directories(http_server_example PUBLIC . ../src ../src/utils)
