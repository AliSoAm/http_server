cmake_minimum_required (VERSION 3.3)
project (example)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-g -std=c++14 -pthread")
find_package (Threads)

add_library(http_server STATIC
            ../src/http_server/http.cpp
            ../src/http_server/http_request.cpp
            ../src/http_server/http_server.cpp
            ../src/http_server/tcp_server.cpp
            ../src/http_server/utils/base64.cpp
            ../src/controllers/controller.cpp
            ../src/controllers/file_controller.cpp
            ../src/controllers/view_controller.cpp)
target_link_libraries (http_server  ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(http_server PUBLIC ../src/http_server ../modules/json/include)

add_executable (http_server_example
                main.cpp)
target_link_libraries (http_server_example  ${CMAKE_THREAD_LIBS_INIT} http_server)
target_include_directories(http_server_example PUBLIC ../src ../modules/json/include)
