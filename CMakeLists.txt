project(fft)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
FIND_PACKAGE(GSL REQUIRED)

add_subdirectory(lib)
include_directories(${GSL_INCLUDE_DIR})
if(UNIX)
    add_definitions(-std=c++0x)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions( -Wall -Wextra -Wformat -g3)
    #add_definitions ( -Wall -Wextra -Wformat -g3 -fstack-check -Weffc++)
  endif(NOT CMAKE_BUILD_TYPE STREQUAL "Release")

endif(UNIX)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} Aquila Ooura_fft ${GSL_LIBRARIES})
