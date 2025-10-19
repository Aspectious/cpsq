add_executable(cpdt cli.cpp)
target_link_libraries(cpdt PRIVATE cpdl)
target_link_directories(cpdt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}../lib)