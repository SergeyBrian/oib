cmake_minimum_required(VERSION 3.21)
project(1_cryptanalysis C)

set(CMAKE_C_STANDARD 99)

set(CURSES_NEED_WIDE TRUE)

find_package(Curses REQUIRED)
message(STATUS "CURSES_LIBRARIES: ${CURSES_LIBRARIES}")
include_directories(${CURSES_INCLUDE_DIR})

add_executable(1_cryptanalysis main.c data.h ui.c ui.h fs.c fs.h utils.c utils.h analysis.c analysis.h data.c)
target_link_libraries(1_cryptanalysis ncursesw)

