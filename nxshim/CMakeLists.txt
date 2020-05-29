cmake_minimum_required(VERSION 2.6)

project(nxshim)
add_subdirectory(glad/cmake)

include_directories(${PROJECT_SOURCE_DIR}/include)

glad_add_library(glad STATIC MERGE API gl:compatibility=4.3)

add_library(nxshim STATIC
		src/glsym.c
		src/glew.c)

target_link_libraries(nxshim glad)