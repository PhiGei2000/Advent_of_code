cmake_minimum_required(VERSION 3.27)
project(Advent_of_code)
set(CMAKE_CXX_STANDARD 23)

foreach (I RANGE 25)
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${I})
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${I})
    endif()
endforeach()