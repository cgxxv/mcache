PROJECT(mcache)
CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -v -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") # -H show all the headers needed for the project.
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++17 -fno-strict-aliasing -Wno-register -Wno-pessimizing-move -Wshadow")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++17 -fno-strict-aliasing -Wno-register -Wno-pessimizing-move -Wshadow")
SET(CMAKE_VERBOSE_MAKEFILE ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DDEBUG_MODE)
endif()

# The version number.
SET(LCACHE_VERSION_MAJOR 1)
SET(LCACHE_VERSION_MINOR 0)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
    "${PROJECT_BINARY_DIR}/src/config.hpp"
    )

FILE(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.hpp"
                    "${PROJECT_SOURCE_DIR}/src/*.h"
                    "${PROJECT_SOURCE_DIR}/src/*.cc")
ADD_LIBRARY(mcache INTERFACE ${sources})

FILE(GLOB sample_srcs "${PROJECT_SOURCE_DIR}/examples/*.hpp"
                        "${PROJECT_SOURCE_DIR}/examples/*.h"
                        "${PROJECT_SOURCE_DIR}/examples/sample.cc")
ADD_EXECUTABLE(sample ${sample_srcs})
TARGET_LINK_LIBRARIES(sample mcache)

FILE(GLOB cache_srcs "${PROJECT_SOURCE_DIR}/examples/*.hpp"
                        "${PROJECT_SOURCE_DIR}/examples/*.h"
                        "${PROJECT_SOURCE_DIR}/examples/mcache.cc")
ADD_EXECUTABLE(cache ${cache_srcs})
TARGET_LINK_LIBRARIES(cache mcache)
