/*
 * Simulate.cpp
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */

#include <DAG/PlainDAG.h>
#include <Simulation/CoreManager.h>
#include <Simulation/DAGScheduler.h>
#include <Simulation/TaskSet.h>
#include <VariableTaskSet/PlainTaskSet.h>
#include <uavAP/Core/DataPresentation/BinarySerialization.hpp>
#include <uavAP/Core/Object/Aggregator.h>
#include <uavAP/Core/Scheduler/MicroSimulator.h>
#include <uavAP/Core/Runner/SimpleRunner.h>

int
main()
{
	std::ifstream file("dag");
	PlainDAG dag = dp::deserialize<PlainDAG>(file);

	std::ifstream fileTasks("tasks");
	auto set = dp::deserialize<PlainTaskSet>(fileTasks);

	std::cout << set << std::endl;

	std::cout << dag.dagMatrix << std::endl;
	std::cout << dag.nodeInfo << std::endl;

	APLogger::instance()->setLogLevel(LogLevel::WARN);
	auto sim = std::make_shared<MicroSimulator>();
	auto dagSched = std::make_shared<DAGScheduler>(dag);
	auto coreMan = std::make_shared<CoreManager>(4);
	auto taskSet = std::make_shared<TaskSet>(set);

	auto agg = Aggregator::aggregate( { sim, dagSched, coreMan, taskSet });

	unsigned imuCount = 0;
	unsigned* imuCounter = &imuCount;
	taskSet->setReadFunction(0, [imuCounter](){(*imuCounter)++; std::cout << *imuCounter << std::endl;});
	taskSet->setWriteFunction(0, [imuCounter](){std::cout << *imuCounter << " done" << std::endl;});

	SimpleRunner runner(agg);

	if (runner.runAllStages())
	{
		APLOG_ERROR << "Something went wrong";
		return 1;
	}

	unsigned totalMillis = 40;
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

}
