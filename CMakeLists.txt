cmake_minimum_required(VERSION 3.25)
project(rosette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DPDK REQUIRED libdpdk)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(rosette STATIC ${SOURCES})

target_include_directories(rosette PUBLIC include ${DPDK_INCLUDE_DIRS})
target_link_libraries(rosette PUBLIC ${DPDK_LIBRARIES})
