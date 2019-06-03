/*
 * SharedResource.hpp
 *
 *  Created on: May 22, 2019
 *      Author: mirco
 */

#ifndef SAMPLES_SHAREDRESOURCE_HPP_
#define SAMPLES_SHAREDRESOURCE_HPP_
#include <DAG/PlainDAG.h>
#include <Evaluation/Evaluation.h>
#include <Evaluation/Scheduling.h>
#include <VariableTaskSet/VariableTaskSet.h>
#include <ctime>
#include <iostream>
#include <uavAP/Core/DataPresentation/BinarySerialization.hpp>

namespace samples
{

inline int
sharedResource()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto in1 = taskSet.addTask(10, 7, "in1");
	auto in2 = taskSet.addTask(10, 6, "in2");
	auto shared = taskSet.addTask(40, 25, "shared");
	auto out1 = taskSet.addTask(20, 2, "out1");
	auto out2 = taskSet.addTask(10, 6, "out2");


	taskSet.addDataEdge(in1, shared, { 0,1,2,3,4 });
	taskSet.addDataEdge(in2, shared, { 0,1,2,3,4 });
	taskSet.addDataEdge(in1, out1, { 0,1 });
	taskSet.addDataEdge(in2, out2, { 0,1 });
	taskSet.addDataEdge(shared, out1, { 0,1,2 });
	taskSet.addDataEdge(shared, out2, { 0,1,2,3,4 });

	taskSet.createBaselineTaskset();

	Evaluation eval;
	eval.addLatency( { in1, shared, out1}, LatencyCost(1, 0),
			LatencyConstraint());
	eval.addLatency( { in2, shared, out2}, LatencyCost(1, 0),
			LatencyConstraint());
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(4));

	const auto& bestDAG = eval.evaluate(taskSet.createDAGs());

	eval.exportReactionTimes("reactions");
	eval.exportDataAges("ages");

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");

	PlainDAG plain(bestDAG, 6);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}


inline int
sharedInput()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto in = taskSet.addTask(10, 7, "in");
	auto proc1 = taskSet.addTask(40, 25, "proc1");
	auto proc2 = taskSet.addTask(20, 10, "proc2");
	auto out1 = taskSet.addTask(20, 12, "out1");
	auto out2 = taskSet.addTask(10, 6, "out2");


	taskSet.addDataEdge(in, proc1);
	taskSet.addDataEdge(in, proc2);
	taskSet.addDataEdge(proc1, out1);
	taskSet.addDataEdge(proc2, out2);

	taskSet.createBaselineTaskset();

	Evaluation eval;
	eval.addLatency( { in, proc1, out1}, LatencyCost(1, 0),
			LatencyConstraint());
	eval.addLatency( { in, proc2, out2}, LatencyCost(1, 0),
			LatencyConstraint());
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(4));

	const auto& bestDAG = eval.evaluate(taskSet.createDAGs());

	eval.exportReactionTimes("reactions");
	eval.exportDataAges("ages");

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");

	PlainDAG plain(bestDAG, 6);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}


inline int
cycles()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto task1 = taskSet.addTask(10, 7, "task1");
	auto task2 = taskSet.addTask(30, 13, "task2");
	auto task3 = taskSet.addTask(30, 10, "task3");

	taskSet.createBaselineTaskset();

	taskSet.addDataEdge(task1, task2, {0,1});
	taskSet.addDataEdge(task2, task3);
	taskSet.addDataEdge(task1, task3);

	Evaluation eval;
	eval.addLatency( { task1, task2, task3}, LatencyCost(1, 1),
			LatencyConstraint(50, 1000000));
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(4));

	const auto& bestDAG = eval.evaluate(taskSet.createDAGs());

	eval.exportReactionTimes("reactions");
	eval.exportDataAges("ages");

	bestDAG.toTikz("prova.tex");
	bestDAG.getOriginatingTaskset()->toTikz("cool.tex");

	PlainDAG plain(bestDAG, 6);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}

inline int
hercules()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto gps = taskSet.addTask(50, 7, "gps");
	auto lidar = taskSet.addTask(50, 12, "lidar");
	auto localization = taskSet.addTask(50, 28, "localization");
	auto detection = taskSet.addTask(50, 28, "detection");
	auto fusion = taskSet.addTask(50, 25, "fusion");
	auto camera = taskSet.addTask(25, 2, "camera");
	auto ekf = taskSet.addTask(10, 6.5, "ekf");
	auto planner = taskSet.addTask(10, 5, "planner");
	auto control = taskSet.addTask(10, 4.5, "control");

	gps->bcet = 5;
	lidar->bcet = 10;
	localization->bcet = 22;
	ekf->bcet = 3;
	planner->bcet = 3.2;
	control->bcet = 1.8;
	camera->bcet = 1.8;
	detection->bcet = 25;
	fusion->bcet = 18.9;

	taskSet.createBaselineTaskset();

	taskSet.addDataEdge(gps, localization);
	taskSet.addDataEdge(lidar, localization);
//	taskSet.addDataEdge(lidar, planner);
	taskSet.addDataEdge(localization, ekf);
	taskSet.addDataEdge(ekf, planner);
	taskSet.addDataEdge(planner, control,0);
	taskSet.addDataEdge(camera, detection, 0);
	taskSet.addDataEdge(detection, fusion);
//	taskSet.addDataEdge(lidar, fusion);
	taskSet.addDataEdge(fusion, planner);

	Evaluation eval;
	eval.addLatency({camera, detection, fusion}, LatencyCost(1,1), LatencyConstraint(120,120));
	eval.addLatency({gps, localization,ekf,planner,control}, LatencyCost(1,1), LatencyConstraint(120,150));
	eval.addLatency({lidar, localization,ekf,planner,control}, LatencyCost(1,1), LatencyConstraint(120,150));
	eval.addLatency({camera, detection, fusion, planner,control}, LatencyCost(1,1), LatencyConstraint(150,150));
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(6));

	const auto& bestDAG = eval.evaluate(taskSet.createDAGs());

	eval.exportReactionTimes("reactions");
	eval.exportDataAges("ages");

	bestDAG.toTikz("hercules_dag.tex");
	bestDAG.getOriginatingTaskset()->toTikz("hercules_taskset.tex");

	PlainDAG plain(bestDAG, taskSet.getPlainTaskSet().name.size());

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}



inline int
simple_example()
{
	time_t tstart, tend;
	tstart = time(0);
	VariableTaskSet taskSet;

	auto task1 = taskSet.addTask(10, 7, "task1");
	auto task2 = taskSet.addTask(30, 13, "task2");
	auto task3 = taskSet.addTask(30, 10, "task3");

	task1->bcet = 5;
	task2->bcet = 10;
	task3->bcet = 8;

	taskSet.createBaselineTaskset();

	taskSet.addDataEdge(task1, task2, {0,1});
	taskSet.addDataEdge(task2, task3);
	taskSet.addDataEdge(task1, task3);

	Evaluation eval;
	eval.addLatency( { task1, task2, task3}, LatencyCost(1, 1.3),
			LatencyConstraint(30, 50));
	eval.addScheduling(SchedulingCost(20), SchedulingConstraint(2));

	const auto& bestDAG = eval.evaluate(taskSet.createDAGs());

	scheduling::scheduleDAG(bestDAG, 2, "simple_schedule.tex");

	eval.exportReactionTimes("reactions");
	eval.exportDataAges("ages");

	bestDAG.toTikz("simple_example.tex");
	bestDAG.getOriginatingTaskset()->toTikz("simple_example_taskset.tex");

	PlainDAG plain(bestDAG, 6);

	std::ofstream file("data");
	dp::serialize(plain, file);
	dp::serialize(taskSet.getPlainTaskSet(), file);
	dp::serialize(eval, file);

	tend = time(0);
	std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;

	return 0;
}

}

#endif /* SAMPLES_SHAREDRESOURCE_HPP_ */
