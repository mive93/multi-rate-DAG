/*
 * Simulate.cpp
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */

#include <DAG/PlainDAG.h>
#include <Simulation/DAGScheduler.h>
#include <Simulation/TaskSet.h>
#include <uavAP/Core/DataPresentation/BinarySerialization.hpp>
#include <uavAP/Core/Scheduler/MicroSimulator.h>


int
main()
{

	std::ifstream file("dag");
	PlainDAG dag = dp::deserialize<PlainDAG>(file);

	std::cout << dag.dagMatrix << std::endl;
	std::cout << dag.nodeInfo << std::endl;

	DAGScheduler dagSched(dag, TaskSet());

	dagSched.taskFinished(0);
	dagSched.reset();

	MicroSimulator sim;

	sim.simulate(Seconds(1));



}
