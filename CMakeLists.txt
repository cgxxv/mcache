PROJECT(mcache)
CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g") # -H show all the headers needed for the project.
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++17 -fno-strict-aliasing -Wno-register -Wno-pessimizing-move -Wshadow")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++17 -fno-strict-aliasing -Wno-register -Wno-pessimizing-move -Wshadow")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_VERBOSE_MAKEFILE ON)

# The version number.
SET(LCACHE_VERSION_MAJOR 1)
SET(LCACHE_VERSION_MINOR 0)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")

CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
    "${PROJECT_BINARY_DIR}/src/config.hpp"
    )

FILE(GLOB mcache_srcs "${PROJECT_SOURCE_DIR}/src/*.hpp")
ADD_LIBRARY(mcache INTERFACE ${mcache_srcs})

FILE(GLOB simple_srcs "${PROJECT_SOURCE_DIR}/examples/*.hpp"
                        "${PROJECT_SOURCE_DIR}/examples/*.cc")
ADD_EXECUTABLE(simple ${simple_srcs})
TARGET_LINK_LIBRARIES(simple mcache)