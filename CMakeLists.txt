cmake_minimum_required(VERSION 3.27)
project(Dndbot)

set(CMAKE_CXX_STANDARD 17)


set(USE_SYSTEM_CURL ON)
add_subdirectory(deps/sleepy-discord)

add_executable(Dndbot ApiClient.cpp)

target_link_libraries(Dndbot PRIVATE sleepy-discord )

target_include_directories(Dndbot PRIVATE "${CMAKE_SOURCE_DIR}/include")