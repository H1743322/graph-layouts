cmake_minimum_required(VERSION 3.16)
project(graph-layout)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -funroll-loops -Wall -Wextra -Wpedantic")
include_directories(include)

# GLFW
find_package(glfw3 REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# GLAD
file(GLOB_RECURSE GLAD_SOURCES "external/glad/src/*.c")
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC external/glad/include)

# IMGUI
file(GLOB IMGUI_SOURCES
    external/imgui/*.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
list(FILTER CORE_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# lib
add_library(core_lib STATIC ${CORE_SOURCES})
target_include_directories(core_lib PUBLIC
    include
)
target_link_libraries(core_lib PUBLIC
    glad
    OpenGL::GL
    glfw
    imgui
)

# exec
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE core_lib)

