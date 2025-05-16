set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)
# find_package(glfw3 3.3 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Vulkan SDK Include Dir: ${Vulkan_INCLUDE_DIRS}")
    message(STATUS "Vulkan SDK Lib Dir: ${Vulkan_LIBRARIES}")
endif()


file(GLOB VKTT_SRC
    "*.hpp"
    "*.cpp"
)

add_executable(vktt ${VKTT_SRC})

target_compile_features(vktt PRIVATE cxx_std_17)

target_compile_options(vktt PRIVATE
    # -fcolor-diagnostics # colored diagnostics for clang
    # -Wall -Wextra # enable warning
    # -fsanitize=address # enable AddressSanitizer
)

# target_include_directories(vktt PRIVATE "/path/to/include")

target_link_libraries(vktt PRIVATE 
    Vulkan::Vulkan 
    ${CMAKE_DL_LIBS} 
    # glfw3Config.cmake or 
    # /usr/lib/cmake/glfw3/glfw3Targets.cmake
    # add_library(glfw3::glfw3 SHARED IMPORTED)
    # glfw for Arch Linux, some other package managers - glfw3 or from source - glfw3::glfw3
    glfw 
    glm::glm 
    Threads::Threads
)
