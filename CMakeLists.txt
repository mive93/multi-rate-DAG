cmake_minimum_required(VERSION 3.5)
project(multiRateDAG CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")

#-------------------------------------------------------------------------------
# External Libraries
#-------------------------------------------------------------------------------
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

#-------------------------------------------------------------------------------
# Submodules
#-------------------------------------------------------------------------------

add_subdirectory(DAG-scheduling)

#-------------------------------------------------------------------------------
# Build libraries
#-------------------------------------------------------------------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/multi-rate-DAG
                    ${CMAKE_CURRENT_SOURCE_DIR}/DAG-scheduling/include)

file(GLOB multiRateDAG-SRC "src/*/*.cpp" "src/*.cpp")
add_library(multiRateDAG SHARED ${multiRateDAG-SRC})
target_link_libraries(multiRateDAG ${multiRateDAG-LIBS} dag-sched)

#-------------------------------------------------------------------------------
# Build executables
#-------------------------------------------------------------------------------
add_executable(demoMRD app/main.cpp)
target_link_libraries(demoMRD multiRateDAG)