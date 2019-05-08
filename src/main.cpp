/*
 * main.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#include <DAG/MaxProduct.h>
#include <Evaluation/Evaluation.h>
#include <VariableTaskSet/VariableTaskSet.h>
#include <eigen3/Eigen/Core>
#include <iostream>
#include "MultiRate/MultiRateTaskset.h"

#include <set>

#include "MultiRate/DummyNodes.h"

#include "Evaluation/Scheduling.h"
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

	auto task1 = taskSet.addTask(5, 1.9, "imu");
	auto task2 = taskSet.addTask(10, 1.2, "gps");
	auto task3 = taskSet.addTask(10, 1.5, "planner");
	auto task4 = taskSet.addTask(5, 1.4, "controller");
	auto task5 = taskSet.addTask(20, 3, "act");
	//auto task6 = taskSet.addTask(40, 5, "train");
	//auto task7 = taskSet.addTask(160, 50, "independent");

	taskSet.addDataEdge(task3, task4, 0);

	taskSet.addDataEdge(task1, task3, 0);
	taskSet.addDataEdge(task1, task4, 1);
	taskSet.addDataEdge(task2, task3, 1);
	//taskSet.addDataEdge(task2, task4, 2);
	//taskSet.addDataEdge(task4, task5, 1);
	//taskSet.addDataEdge(task1, task6, 6);
	//taskSet.addDataEdge(task2, task6, 0);


	taskSet.createBaselineDAG();

	auto dags = taskSet.createDAGs();

	if (dags.empty())
		return 1;

	float numEdges = 10000;
	unsigned id = 0;
	unsigned k = 0;
	int n_processors = 4;

	for (auto& dag : dags)
	{
		auto info = dag.getLatencyInfo( { 0, 0, 2, 3, 4 });
		if (info.reactionTime < numEdges)
		{
			numEdges = info.reactionTime;
			id = k;
		}
		k++;

		scheduling::scheduleDAG(dag,n_processors);
	}

	dags[id].toTikz("prova.tex");
	scheduling::scheduleDAG(dags[id],n_processors,"schedule_test.tex", true);
	dags[id].getOriginatingTaskset()->toTikz("cool.tex");
	std::cout << dags[id].getNodeInfo() << std::endl;
	std::cout << dags[id].getLatencyInfo( { 0,0,4 }) << std::endl;
	dags[id].getLatencyInfoIterative( { 1,1,5});

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
multiTaskset()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto task1 = taskSet.addTask(5, 2.5, "imu");
	auto task2 = taskSet.addTask(20, 5.2, "gps");
	auto task3 = taskSet.addTask(10, 5.4, "planner");
	auto task4 = taskSet.addTask(10, 8, "controller");
	auto task5 = taskSet.addTask(20, 9, "act");
	auto task6 = taskSet.addTask(40, 35, "train");
//	auto task7 = taskSet.addTask(80, 50, "independent");

	taskSet.addDataEdge(task3, task4, { 0, 1});

	taskSet.addDataEdge(task1, task3, { 0, 1, 2 });
	taskSet.addDataEdge(task1, task4, { 0, 1, 2 });
	taskSet.addDataEdge(task2, task3, { 0, 1, 2 });
	taskSet.addDataEdge(task2, task4, { 0, 1, 2 });
	taskSet.addDataEdge(task4, task5, { 0, 1, 2 });
//	taskSet.addDataEdge(task1, task6,  { 7, 8 });
	taskSet.addDataEdge(task2, task6,  { 2 });
//	taskSet.addDataEdge(task5, task6,  { 1,2 });

	taskSet.createBaselineTaskset();

	auto& allDags = taskSet.createDAGs();

	std::cout << allDags.size() << " total valid DAGs were created" << std::endl;

	Evaluation eval;
	eval.addLatency({task1, task1, task3, task4, task5}, LatencyCost(0,2), LatencyConstraint(50,55));
	eval.addLatency({task2, task2, task3, task4, task5}, LatencyCost(0,3), LatencyConstraint(60,60));
	eval.addLatency({task3, task4}, LatencyCost(), LatencyConstraint(20, 20));
//	eval.addLatency({task1, task6}, LatencyCost(), LatencyConstraint(40, 100));
	eval.addScheduling(SchedulingCost(), SchedulingConstraint(4));

	const auto& bestDAG = eval.evaluate(allDags);

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");
	scheduling::scheduleDAG(bestDAG, 4, "schedule_test.tex");


	std::cout << bestDAG.getNodes().back()->executionTimeGen() << std::endl;
	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

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

