cmake_minimum_required(VERSION 2.6)

project(nxshim)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/libretro-common/include)

add_library(nxshim STATIC
		libretro-common/glsym/glsym_gl.c
		libretro-common/glsym/rglgen.c
		src/glsym.c
		src/glew.c)