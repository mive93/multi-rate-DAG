cmake_minimum_required (VERSION 2.8)
project (dags)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -std=c++14 -fPIC -Wall -Wno-int-in-bool-context")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall")

include_directories(${CMAKE_SOURCE_DIR})

set(VARIABLE_SRCS	VariableTaskSet/VariableTaskSet.cpp
					VariableTaskSet/VariableMultiEdge.cpp
					)

set(MULTIRATE_SRCS	MultiRate/MultiNode.cpp
					MultiRate/MultiEdge.cpp
					MultiRate/MultiRateTaskset.cpp
					MultiRate/DummyNodes.cpp
					)
					
set(DAG_SRCS	DAG/DAG.cpp
				DAG/Edge.cpp
				DAG/MaxProduct.cpp
				)
					
set(EVAL_SRCS	Evaluation/Evaluation.cpp
				Evaluation/Scheduling.cpp
				)

add_executable(main main.cpp
					${VARIABLE_SRCS}
					${MULTIRATE_SRCS}
					${DAG_SRCS}
					${EVAL_SRCS}
					)
					
					
					
add_executable(sim Simulation/Simulate.cpp)

target_link_libraries(sim uavap_core boost_system)