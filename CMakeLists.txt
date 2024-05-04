cmake_minimum_required(VERSION 3.28)
project(Shunting_Yard C)

set(CMAKE_C_STANDARD 23)

add_executable(Shunting_Yard src/main.c
        vendor/sc/sc_map.h
        vendor/sc/sc_map.c
        vendor/sc/sc_queue.h
        src/shunting_yard.h
        src/shunting_yard.c
)

target_include_directories(Shunting_Yard PRIVATE
        src
        vendor)
