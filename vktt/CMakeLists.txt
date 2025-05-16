cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

project(VulkanTutorial)

enable_language(C CXX)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang++")
# if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#     message(WARNING ${CMAKE_CXX_COMPILER_ID})
#     message(WARNING "clang++ is recommended for this project!")
# endif()

add_subdirectory(src)
