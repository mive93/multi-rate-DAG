/*
 * Simulate.cpp
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */

#include <DAG/PlainDAG.h>
#include <Evaluation/Evaluation.h>
#include <Simulation/CoreManager.h>
#include <Simulation/DAGScheduler.h>
#include <Simulation/TaskSet.h>
#include <VariableTaskSet/PlainTaskSet.h>
#include <uavAP/Core/DataPresentation/BinarySerialization.hpp>
#include <uavAP/Core/Object/Aggregator.h>
#include <uavAP/Core/Scheduler/MicroSimulator.h>
#include <uavAP/Core/Runner/SimpleRunner.h>

int
main(int argc, char** argv)
{

	unsigned seed = 0;
	if (argc == 2)
	{
		seed = atoi(argv[1]);
	}

	std::ifstream file("data");
	auto dag = dp::deserialize<PlainDAG>(file);
	auto set = dp::deserialize<PlainTaskSet>(file);
	auto evalRead = dp::deserialize<Evaluation>(file);

	dag.getLatencyInfo({0});

	APLogger::instance()->setLogLevel(LogLevel::WARN);
	auto sim = std::make_shared<MicroSimulator>();
	auto dagSched = std::make_shared<DAGScheduler>(dag);
	auto coreMan = std::make_shared<CoreManager>(4);
	auto taskSet = std::make_shared<TaskSet>(set);
	auto eval = std::make_shared<Evaluation>(evalRead);

	eval->addChain({1,0,0,2,3,4});

	eval->printInfo();
	taskSet->setSeed(seed);

	auto agg = Aggregator::aggregate( { sim, dagSched, coreMan, taskSet, eval });

	SimpleRunner runner(agg);

	if (runner.runAllStages())
	{
		APLOG_ERROR << "Something went wrong";
		return 1;
	}

	unsigned totalMillis = 1e5;
	unsigned increment = 2;
	unsigned time = 0;
	float realTime = 0;

	if (realTime > 0)
		while (time < totalMillis)
		{
			APLOG_TRACE << sim->now().time_of_day();
			sim->simulate(Milliseconds(increment));
			time += increment;
			std::this_thread::sleep_for(
					std::chrono::microseconds(static_cast<int>(realTime * increment * 1000)));
		}
	else
		sim->simulate(Milliseconds(totalMillis));

	eval->exportLatency("test1");


}
