add_library(cpdl STATIC library.cpp library.h)
target_include_directories(cpdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})