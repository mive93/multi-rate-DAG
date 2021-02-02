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
#include <cpsCore/Utilities/Scheduler/MicroSimulator.h>
#include <cpsCore/Synchronization/SimpleRunner.h>
#include <cpsCore/Aggregation/Aggregator.h>
#include <cpsCore/Utilities/DataPresentation/BinarySerialization.hpp>
#include <thread>

int
main(int argc, char** argv)
{

	unsigned seed = 0;
	if (argc == 2)
	{
		seed = atoi(argv[1]);
	}
	else
		seed = time(NULL);

	std::ifstream file("data");
	auto dag = dp::deserialize<PlainDAG>(file);
	auto set = dp::deserialize<PlainTaskSet>(file);
	auto evalRead = dp::deserialize<Evaluation>(file);

	std::cout << dag.nodeInfo << std::endl;

	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
	auto sim = std::make_shared<MicroSimulator>();
	auto dagSched = std::make_shared<DAGScheduler>(dag);
	auto coreMan = std::make_shared<CoreManager>(6);
	auto taskSet = std::make_shared<TaskSet>(set);
	auto eval = std::make_shared<Evaluation>(evalRead);

	eval->printInfo();
	taskSet->setSeed(seed);

//	eval->addJitterCount(7,8);
//	eval->addJitterCount(6,7);
//	eval->addJitterCount(5,3);

	auto agg = Aggregator::aggregate( { sim, dagSched, coreMan, taskSet, eval });

	SimpleRunner runner(agg);

	if (runner.runAllStages())
	{
		CPSLOG_ERROR << "Something went wrong";
		return 1;
	}

	unsigned totalMillis = 1e9;
	unsigned increment = 1e5;
	unsigned time = 0;
	float realTime = 1e-10;

	if (realTime > 0)
		while (time < totalMillis)
		{
			std::cout << std::chrono::duration_cast<Milliseconds>(sim->now() - TimePoint()).count() << std::endl;
			sim->simulate(Milliseconds(increment));
			time += increment;
			std::this_thread::sleep_for(
					std::chrono::microseconds(static_cast<int>(realTime * increment * 1000)));
		}
	else
		sim->simulate(Milliseconds(totalMillis));

//	eval->exportLatency("hercules");
//	eval->exportJitterCount("hercules");

}
