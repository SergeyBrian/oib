cmake_minimum_required(VERSION 3.23)
project(1_cryptanalysis C)

set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(1_cryptanalysis main.c data.h)
target_link_libraries(1_cryptanalysis ${CURSES_LIBRARIES})
