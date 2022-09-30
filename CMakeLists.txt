cmake_minimum_required(VERSION 3.23)
project(GlExample)

set(CMAKE_CXX_STANDARD 14)

# glfw
set(GLFW_DIR third_party/glfw)
add_subdirectory(${GLFW_DIR})

#glad
set(GLAD_DIR third_party/glad)
include_directories(${GLAD_DIR}/include)

add_executable(GlExample src/main.cpp ${GLAD_DIR}/src/glad.c)
target_link_libraries(GlExample glfw)
