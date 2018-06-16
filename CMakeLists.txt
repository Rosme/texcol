cmake_minimum_required(VERSION 3.1)

project(TexCol)

SET(SFML_DIR ${SFML_CMAKE_PATH})

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)