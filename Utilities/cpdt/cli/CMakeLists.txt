add_executable(cli cli.cpp)
target_link_libraries(cli PRIVATE lib)
target_link_directories(cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}../lib)