cmake_minimum_required(VERSION 2.6)

project(stepmania-nx)

include_directories(nxshim/include)
include_directories("${CMAKE_BINARY_DIR}/nxshim/gladsources/glad/include")

add_subdirectory(nxshim)
add_subdirectory(stepmania)