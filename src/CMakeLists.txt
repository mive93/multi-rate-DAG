cmake_minimum_required (VERSION 2.8)
project (dags)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -std=c++17 -fPIC -Wall -Wno-int-in-bool-context")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -O3")


find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


include_directories(${CMAKE_SOURCE_DIR}
					"/usr/local/include/eigen3/")

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

set(BENCH_SRCS	Benchmark/DataFiles.cpp
				Benchmark/Becker.cpp
				Benchmark/Benchmark.cpp
				)
			
					
set(SIM_SRCS	Simulation/TaskSet.cpp
				Simulation/Task.cpp
				Simulation/DAGScheduler.cpp
				Simulation/CoreManager.cpp
				Simulation/ChainSim.cpp
				Simulation/JitterCount.cpp
				)
				
				
add_library(dag 	${VARIABLE_SRCS}
					${MULTIRATE_SRCS}
					${DAG_SRCS}
					${EVAL_SRCS}
					${BENCH_SRCS}
					${SIM_SRCS}
					)

add_executable(main main.cpp
					)

target_link_libraries(main uavap_core dag)
					
					
					
add_executable(sim 	Simulation/Simulate.cpp
					)

target_link_libraries(sim uavap_core boost_system dag)
