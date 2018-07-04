cmake_minimum_required(VERSION 3.1)

project(TexCol)

SET(SFML_DIR ${SFML_CMAKE_PATH})

include_directories(imgui)
include_directories(imgui-sfml)
include_directories(rsm)

set(SRC 
	#Code
    main.cpp
	texcol.hpp
	texcol.cpp
	shadereffect.hpp
	shadereffect.cpp
	controlwindow.hpp
	controlwindow.cpp
	sansation.hpp
	#Imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui-sfml/imgui-SFML.cpp
    )

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio opengl32)