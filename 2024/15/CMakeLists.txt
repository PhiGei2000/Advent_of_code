set(PROJECT_NAME 15-Warehouse-Woes)

add_executable(${PROJECT_NAME} main.cpp)

file(CREATE_LINK ${CMAKE_CURRENT_LIST_DIR}/input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt)