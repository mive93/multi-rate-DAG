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

	auto task1 = taskSet.addTask(5, 3, "imu");
	auto task2 = taskSet.addTask(20, 10, "gps");
	auto task3 = taskSet.addTask(10, 3, "planner");
	auto task4 = taskSet.addTask(10, 6, "controller");
//	auto task5 = taskSet.addTask(20, 2, "act");
//	auto task6 = taskSet.addTask(40, 15, "train");
//	auto task7 = taskSet.addTask(160, 50, "independent");

	taskSet.addPrecedenceEdge(task3, task4);

	taskSet.addDataEdge(task1, task3, 2);
	taskSet.addDataEdge(task1, task4, 2);
	taskSet.addDataEdge(task2, task3, 1);
	taskSet.addDataEdge(task2, task4, 1);
//	taskSet.addDataEdge(task4, task5, 0);
//	taskSet.addDataEdge(task1, task6, 6);
//	taskSet.addDataEdge(task2, task6, 0);
//
	taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();

	if (dags.empty())
		return 1;

	unsigned numEdges = 1000000;
	unsigned id = 0;
	unsigned k = 0;

	for (auto& dag : dags)
	{
		//dag.createNodeInfo();
		if (dag.getEdges().size() < numEdges)
		{
			numEdges = dag.getEdges().size();
			id = k;
		}
		k++;
	}

	dags[id].toTikz("prova.tex");
	dags[id].createNodeInfo();
	dags[id].toTikz("prova2.tex");


//	std::cout << dags[id].getJitterMatrix() << std::endl;

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
//
//	Eigen::Matrix<int, 5, 1> v1;
//	Eigen::Matrix<int, 5, 5> v2 = Eigen::Matrix<int, 5, 5>::Random();
//
//	v1 << 0,0,1,1,0;
//
//	std::cout << v1.asDiagonal() * v2 << std::endl;


}

