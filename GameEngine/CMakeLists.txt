cmake_minimum_required(VERSION 4.0)
project(cpsq)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# External Libraries and Headers
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
add_library(glfw STATIC IMPORTED)
add_library(cpdl STATIC IMPORTED)
set_target_properties(cpdl PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/cpdl.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/lib/include"
)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib" # Adjust path
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/lib/include/" # Adjust path
)
add_library(glad STATIC IMPORTED)
set_target_properties(glad PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/glad.lib" # Adjust path
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/lib/include/" # Adjust path
)

# Create Program
set(MY_SOURCES
        src/render/opengl.renderer.cpp
        src/main.cpp
        include/cpsq.h
        src/render/opengl.h
        src/render/cpsq.render.opengl.h/cpsq.render.opengl.h
        src/render/opengl.renderer.cpp
)

add_executable(cpsq ${MY_SOURCES})

target_include_directories(cpsq PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# Link GLFW to your executable
target_link_libraries(cpsq PRIVATE glfw opengl32 glm::glm glad cpdl) # Assuming you also use OpenGL

