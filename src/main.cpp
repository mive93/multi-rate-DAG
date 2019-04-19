/*
 * main.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

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

	const auto& baseline = taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();

	dags[2].toTikz("test.tex");

}

int
taskset3()
{
	MultiRateTaskset taskSet;

	auto task1 = taskSet.addTask(5, 2, "imu");
	auto task2 = taskSet.addTask(20, 4, "gps");
	auto task3 = taskSet.addTask(10, 3, "planner");
	auto task4 = taskSet.addTask(10, 3, "controller");
	auto task5 = taskSet.addTask(20, 2, "act");
//	auto task6 = taskSet.addTask(40, 27, 30, "train");

	taskSet.addPrecedenceEdge(task3, task4);

	taskSet.addDataEdge(task1, task3, 1);
	taskSet.addDataEdge(task1, task4, 1);
	taskSet.addDataEdge(task2, task3, 0);
	taskSet.addDataEdge(task2, task4, 0);
	taskSet.addDataEdge(task4, task5, 0);
//
//	taskSet.addDataEdge(task1, task6, 8);
//	taskSet.addDataEdge(task2, task6, 1);

	const auto& baseline = taskSet.createBaselineDAG();

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

	int identDAG = 2;
	if (!taskSet.checkJitter(dags[identDAG]))
		std::cout << "Jitter not correct" << std::endl;
	dags[identDAG].toTikz("prova.tex");

	return 0;

}

void
taskset2()
{
	MultiRateTaskset taskSet;

	auto task1 = taskSet.addTask(10, 2, "sensor");
	auto task2 = taskSet.addTask(10, 5, "sensor");
	auto task3 = taskSet.addTask(20, 5, "act");

	taskSet.addDataEdge(task1, task2, 2);
//	taskSet.addDataEdge(task2, task3, 1);

//	taskSet.addPrecedenceEdge(task1, task2);

	auto baseline = taskSet.createBaselineDAG();

	baseline.printNodes();
	baseline.printEdges();

	auto dags = taskSet.createDAGs();

	for (const auto& dag : dags)
	{
		std::cout << "Next DAG:" << std::endl;
		dag.printEdges();
		std::cout << std::endl;
	}
}

int
main(int argc, char** argv)
{
	return taskset3();

}
