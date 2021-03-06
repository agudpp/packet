
cmake_minimum_required(VERSION 2.8)

# project name
project(packet)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


################################################################################
# check the configuration and unify if
set(DEBUG_MODE false)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(DEBUG_MODE true)
    message("WE ARE IN DEBUG MODE")
endif ()


# source root dir
set(PRJ_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(SRC_ROOT_DIR ${PRJ_ROOT_DIR}/src)
set(INCLUDE_ROOT_DIR ${PRJ_ROOT_DIR}/include)


################################################################################
# define global variables for headers | sources | libs | etc

set(COMMON_CXX_FLAGS "-std=c++14 -Wall ")
if (DEBUG_MODE)
    set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -ggdb3")
    set(COMMON_DEFINITIONS "${COMMON_DEFINITIONS} -DDEBUG -DUSE_DEBUG")
else()
    set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -O3 -fwrapv")
    set(COMMON_DEFINITIONS "${COMMON_DEFINITIONS}")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_CXX_FLAGS}")
add_definitions(${COMMON_DEFINITIONS})


include_directories(${INCLUDE_ROOT_DIR})

# source

set(HEADERS_LIST ${HEADERS_LIST}
  ${INCLUDE_ROOT_DIR}/packet/debug_helper.h
  ${INCLUDE_ROOT_DIR}/packet/defs.h
  ${INCLUDE_ROOT_DIR}/packet/buffer_part.h
  ${INCLUDE_ROOT_DIR}/packet/buffer_part_impl.h
  ${INCLUDE_ROOT_DIR}/packet/packet.h
  ${INCLUDE_ROOT_DIR}/packet/packet_impl.h
  ${INCLUDE_ROOT_DIR}/packet/packet_helper.h
)

add_executable(${PROJECT_NAME} src/test.cpp ${HEADERS_LIST})
