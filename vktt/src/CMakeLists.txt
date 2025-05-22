set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
# find_package (tinyobjloader CONFIG REQUIRED)
find_package(PkgConfig)

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
    glfw
    glm::glm
    Threads::Threads
    # tinyobjloader::tinyobjloader
    tinyobjloader
)
