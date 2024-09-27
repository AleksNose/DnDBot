cmake_minimum_required(VERSION 3.27)
project(Dndbot)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED VCPKG)
    message(STATUS "VCPKG not defined. Please set it to the location of your vcpkg installation.")
else()
    message(STATUS "Using vcpkg from ${VCPKG}")
    
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()

find_package(cpprestsdk CONFIG REQUIRED)

find_package(sleepy-discord CONFIG REQUIRED)

add_executable(Dndbot ApiClient.cpp)

target_link_libraries(Dndbot PRIVATE sleepy-discord cpprestsdk::cpprest)

target_include_directories(Dndbot PRIVATE "${CMAKE_SOURCE_DIR}/include")