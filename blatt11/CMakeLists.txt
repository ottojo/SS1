cmake_minimum_required(VERSION 2.8.9)
project (short_trips)

set(CMAKE_C_STANDARD 11)
include_directories(/opt/diet/include/libowfat)
link_directories(/opt/diet/lib)
add_executable(short_trips short_trips.c)
target_link_libraries(short_trips libowfat.a m)