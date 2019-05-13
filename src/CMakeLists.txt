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
				DAG/PlainDAG.cpp
				)
					
set(EVAL_SRCS	Evaluation/Evaluation.cpp
				Evaluation/Scheduling.cpp
				)
					
set(SIM_SRCS	Simulation/TaskSet.cpp
				Simulation/DAGScheduler.cpp
				)
				
				
add_library(dag 	${VARIABLE_SRCS}
					${MULTIRATE_SRCS}
					${DAG_SRCS}
					${EVAL_SRCS}
					)

add_executable(main main.cpp
					)

target_link_libraries(main uavap_core dag)
					
					
					
add_executable(sim 	Simulation/Simulate.cpp
					${SIM_SRCS})

target_link_libraries(sim uavap_core boost_system dag)