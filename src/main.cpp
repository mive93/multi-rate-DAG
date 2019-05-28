/*
 * main.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#include <DAG/MaxProduct.h>
#include <DAG/PlainDAG.h>
#include <Evaluation/Evaluation.h>
#include <samples/SharedResource.hpp>
#include <VariableTaskSet/VariableTaskSet.h>
#include <eigen3/Eigen/Core>
#include <iostream>
#include "MultiRate/MultiRateTaskset.h"

#include <set>

#include "MultiRate/DummyNodes.h"

#include "Evaluation/Scheduling.h"
#include <algorithm>

#include <uavAP/Core/DataPresentation/BinarySerialization.hpp>

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
	auto task2 = taskSet.addTask(10, 2, "gps");
	auto task3 = taskSet.addTask(10, 2, "planner");
	auto task4 = taskSet.addTask(5, 2, "controller");
	auto task5 = taskSet.addTask(20, 10, "act");

	task5->bcet = 5;
	task1->bcet = 1;
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

		scheduling::scheduleDAG(dag, n_processors);
	}

	dags[id].toTikz("prova.tex");
	scheduling::scheduleDAG(dags[id], n_processors, "schedule_test.tex", true);
	dags[id].getOriginatingTaskset()->toTikz("cool.tex");
	std::cout << dags[id].getNodeInfo() << std::endl;
	std::cout << dags[id].getLatencyInfo( { 0, 4 }) << std::endl;
	//dags[id].getLatencyInfoIterative( { 1,5,3,4});

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

	auto task1 = taskSet.addTask(5, 4, "imu");
	auto task2 = taskSet.addTask(60, 5.2, "gps");
	auto task3 = taskSet.addTask(10, 4.8, "planner");
	auto task4 = taskSet.addTask(10, 7, "controller");
	auto task5 = taskSet.addTask(20, 8, "act");
//	auto task6 = taskSet.addTask(30, 21, "train");

	task1->bcet = 2;
	task2->bcet = 3.1;
	task3->bcet = 2.4;
	task4->bcet = 6.5;
	task5->bcet = 4.7;
//	task6->bcet = 15;
//	task3->bcet = 4;
//	auto task7 = taskSet.addTask(80, 50, "independent");

	taskSet.addDataEdge(task3, task4, { 0, 1 });

	taskSet.addDataEdge(task1, task3, { 0, 1, 2 });
	taskSet.addDataEdge(task1, task4, { 0, 1, 2 });
	taskSet.addDataEdge(task2, task3, { 0, 1, 2 });
	taskSet.addDataEdge(task2, task4, { 0, 1, 2 });
	taskSet.addDataEdge(task4, task5, { 0, 1, 2 });
//	taskSet.addDataEdge(task1, task6, { 0, 1, 2 });
//	taskSet.addDataEdge(task2, task6, { 0, 1, 2 });
//	taskSet.addDataEdge(task4, task6, { 0, 1, 2, 3 });

	taskSet.createBaselineTaskset();

	auto& allDags = taskSet.createDAGs();

	std::cout << allDags.size() << " total valid DAGs were created" << std::endl;

	Evaluation eval;
	eval.addLatency( { task1, task3, task4, task5 }, LatencyCost(1, 15),
			LatencyConstraint(200, 200));
	eval.addLatency( { task2, task3, task4, task5 }, LatencyCost(1, 3),
			LatencyConstraint(150, 150));
//	eval.addLatency( { task1, task4, task6 }, LatencyCost(1, 3), LatencyConstraint(150, 150));
//	eval.addLatency({task1, task3}, LatencyCost(1,1), LatencyConstraint(25,25));
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(6));

	const auto& bestDAG = eval.evaluate(allDags);

	eval.exportReactionTimes("reactions");

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");
	std::cout << bestDAG.getNodeInfo() << std::endl;
	bestDAG.getLatencyInfo( { 1, 1, 2, 3, 4 });
//	scheduling::scheduleDAG(bestDAG, 4, "schedule_test.tex");

	PlainDAG plain(bestDAG, 6);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}

int
multiTaskset2()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto task1 = taskSet.addTask(10, 4, "imu");
	auto task2 = taskSet.addTask(20, 6, "planner");

	task2->bcet = 3;

	taskSet.addDataEdge(task1, task2, { 0, 1, 2 });

	taskSet.createBaselineTaskset();

	auto& allDags = taskSet.createDAGs();

	std::cout << allDags.size() << " total valid DAGs were created" << std::endl;

	Evaluation eval;
	eval.addLatency( { task1, task2 }, LatencyCost(1, 1), LatencyConstraint(60, 60));

	const auto& bestDAG = eval.evaluate(allDags);

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");

	PlainDAG plain(bestDAG, 2);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	std::cout << bestDAG.getNodeInfo() << std::endl;

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}

int
multiTasksetPareto()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto task1 = taskSet.addTask(10, 4, "coffee");
	auto task2 = taskSet.addTask(40, 32, "food");
	auto task3 = taskSet.addTask(10, 7.2, "think");
	auto task4 = taskSet.addTask(20, 7, "move");
	auto task5 = taskSet.addTask(40, 8, "poop");
//	auto task6 = taskSet.addTask(30, 21, "train");

	task1->bcet = 2;
	task2->bcet = 3.1;
	task3->bcet = 2.4;
	task4->bcet = 6.5;
	task5->bcet = 4.7;


	taskSet.addDataEdge(task1, task3, { 0, 1 });
	taskSet.addDataEdge(task2, task3, { 0, 1, 2, 3, 4});
	taskSet.addDataEdge(task2, task4, { 0, 1, 2 });
	taskSet.addDataEdge(task4, task5, { 0, 1, 2 });
	taskSet.addDataEdge(task1, task5, { 0,1,2,3,4 });
	taskSet.addDataEdge(task2, task5, { 0,1 });

	taskSet.createBaselineTaskset();

	auto& allDags = taskSet.createDAGs();

	std::cout << allDags.size() << " total valid DAGs were created" << std::endl;

	Evaluation eval;
	eval.addLatency( { task1, task5, task2}, LatencyCost(1, 1),
			LatencyConstraint(200, 200));
	eval.addLatency( { task2, task3, task4, task5 }, LatencyCost(1, 1),
			LatencyConstraint(150, 150));
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(4));

	const auto& bestDAG = eval.evaluate(allDags);

	eval.exportReactionTimes("reactions");
	eval.exportDataAges("ages");

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");
	std::cout << bestDAG.getNodeInfo() << std::endl;

	PlainDAG plain(bestDAG, 6);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}



int
main()
{

	return multiTaskset();

}

