cmake_minimum_required (VERSION 2.8)
project (dags)

include_directories(${CMAKE_SOURCE_DIR})

set(MULTIRATE_SRCS	MultiRate/MultiNode.cpp
					MultiRate/MultiEdge.cpp
					MultiRate/MultiRateTaskset.cpp
					)
					
set(DAG_SRCS	DAG/DAG.cpp
				DAG/Node.cpp
				DAG/Edge.cpp
				)

add_executable(main main.cpp
					${MULTIRATE_SRCS}
					${DAG_SRCS}
					)