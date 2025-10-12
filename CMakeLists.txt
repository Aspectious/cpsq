cmake_minimum_required(VERSION 4.0)
project(cpsq)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib" # Adjust path
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/" # Adjust path
)
add_library(glad STATIC IMPORTED)
set_target_properties(glad PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/glad.lib" # Adjust path
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/" # Adjust path
)

add_executable(cpsq main.cpp)

# Link GLFW to your executable
target_link_libraries(cpsq PRIVATE glfw opengl32 glm::glm glad) # Assuming you also use OpenGL

