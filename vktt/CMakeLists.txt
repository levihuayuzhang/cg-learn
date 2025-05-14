cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

project(vulkanTest)

set(CMAKE_CXX_STANDARD 17)

# find_package(glfw3 3.3 REQUIRED)
# find_package(glm REQUIRED)
# find_package(Vulkan REQUIRED)
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

add_executable(VulkanTest main.cpp)

target_link_libraries(VulkanTest glfw vulkan glm ${CMAKE_DL_LIBS} pthread)
