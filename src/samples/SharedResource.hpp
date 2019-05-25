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

}

#endif /* SAMPLES_SHAREDRESOURCE_HPP_ */