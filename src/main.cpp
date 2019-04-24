/*
 * main.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#include <DAG/MaxProduct.h>
#include <eigen3/Eigen/Core>
#include <iostream>
#include "MultiRate/MultiRateTaskset.h"

#include <set>

#include "MultiRate/DummyNodes.h"
void
taskset1()
{
	MultiRateTaskset taskSet;

	auto task1 = taskSet.addTask(10, 2, "sensor1");
	auto task2 = taskSet.addTask(10, 4, "sensor2");
	auto task3 = taskSet.addTask(20, 5, "controller");
	auto task4 = taskSet.addTask(20, 3, "actuator");

	taskSet.addPrecedenceEdge(task3, task4);
	taskSet.addDataEdge(task1, task3, 0);
	taskSet.addDataEdge(task2, task3, 1);

	taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();

	dags[2].toTikz("test.tex");

}

int
taskset3()
{
	time_t tstart, tend;
	tstart = time(0);
	MultiRateTaskset taskSet;

	auto task1 = taskSet.addTask(5, 2, "imu");
	auto task2 = taskSet.addTask(20, 4, "gps");
	auto task3 = taskSet.addTask(10, 3, "planner");
	auto task4 = taskSet.addTask(10, 3, "controller");
	auto task5 = taskSet.addTask(20, 2, "act");
//	auto task6 = taskSet.addTask(40, 25, "train");

	taskSet.addPrecedenceEdge(task3, task4);

	taskSet.addDataEdge(task1, task3, 0);
	taskSet.addDataEdge(task1, task4, 0);
	taskSet.addDataEdge(task2, task3, 0);
	taskSet.addDataEdge(task2, task4, 0);
	taskSet.addDataEdge(task4, task5, 0);
//	taskSet.addDataEdge(task1, task6, 6);
//	taskSet.addDataEdge(task2, task6, 1);

	taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();

	if (dags.empty())
		return 1;

	unsigned numEdges = 1000000;
	unsigned id = 0;
	unsigned k = 0;

	for (const auto& dag : dags)
	{
		if (dag.getEdges().size() < numEdges)
		{
			numEdges = dag.getEdges().size();
			id = k;
		}
		k++;
	}

	dags[id].toTikz("prova.tex");

	dags[id].createNodeInfo();

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;

}

int
taskset2()
{
	MultiRateTaskset taskSet;

	auto task1 = taskSet.addTask(10, 9, "sensor1");
	auto task2 = taskSet.addTask(10, 5, "sensor2");
	auto task3 = taskSet.addTask(20, 2, "act");

	taskSet.addDataEdge(task1, task2, 1);
	taskSet.addDataEdge(task1, task3, 0);

//	taskSet.addPrecedenceEdge(task1, task2);

	taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();


	return 0;
}

int
main()
{
	return taskset3();

//	Eigen::Matrix<int, 5, 1> v1;
//	Eigen::Matrix<int, 5, 1> v2;
//
//	v1 << 1,2,3,4,5;
//	v2 << 2,3,1,3,6;
//
//	auto val = v1.array().max(v2.array());
//	std::cout << val << std::endl;


}

