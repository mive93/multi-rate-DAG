/*
 * main.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#include <iostream>
#include "MultiRate/MultiRateTaskset.h"

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
	taskSet.addDataEdge(task2, task3, 0);

	const auto& dag = taskSet.createBaselineDAG();

	dag.printNodes();
	dag.printEdges();
}

void
taskset2()
{
	MultiRateTaskset taskSet;

	auto task1 = taskSet.addTask(10, 2, "sensor1");
	auto task2 = taskSet.addTask(20, 5, "controller");

	taskSet.addDataEdge(task1, task2, 0);

	const auto& dag = taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();

	dag.printNodes();
	dag.printEdges();
}

int
main(int argc, char** argv)
{
	taskset2();


}
