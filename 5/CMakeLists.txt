set(PROJECT_NAME 5-Print-Queue)

add_executable(${PROJECT_NAME} main.cpp)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/input.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})