cmake_minimum_required(VERSION 3.24)
project(Dndbot)

set(CMAKE_CXX_STANDARD 17)

find_package(cpprestsdk CONFIG REQUIRED)

add_executable(Dndbot ApiClient.cpp)

target_link_libraries(Dndbot PRIVATE cpprestsdk::cpprest)

target_include_directories(Dndbot PRIVATE "${CMAKE_SOURCE_DIR}/include")