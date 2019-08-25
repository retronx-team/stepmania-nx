cmake_minimum_required(VERSION 2.6)

project(stepmania-nx)

include_directories(nxshim/include)
include_directories(nxshim/libretro-common/include)
add_subdirectory(nxshim)
add_subdirectory(stepmania)