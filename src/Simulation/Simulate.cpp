/*
 * Simulate.cpp
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */

#include <DAG/PlainDAG.h>
#include <Simulation/DAGScheduler.h>
#include <Simulation/TaskSet.h>
#include <uavAP/Core/DataPresentation/BinarySerialization.hpp>
#include <uavAP/Core/Object/Aggregator.h>
#include <uavAP/Core/Scheduler/MicroSimulator.h>
#include <uavAP/Core/Runner/SimpleRunner.h>

int
main()
{
	std::ifstream file("dag");
	PlainDAG dag = dp::deserialize<PlainDAG>(file);

	std::cout << dag.dagMatrix << std::endl;
	std::cout << dag.nodeInfo << std::endl;

	APLogger::instance()->setLogLevel(LogLevel::DEBUG);
	auto sim = std::make_shared<MicroSimulator>();
	auto dagSched = std::make_shared<DAGScheduler>(dag);

	auto agg = Aggregator::aggregate({sim, dagSched});

	SimpleRunner runner(agg);

	if (runner.runAllStages())
	{
		APLOG_ERROR << "Something went wrong";
		return 1;
	}

	unsigned totalMillis = 1000;
	unsigned increment = 10;
	unsigned time = 0;
	float realTime = 10;

	while (time < totalMillis)
	{
		APLOG_TRACE << sim->now().time_of_day();
		sim->simulate(Milliseconds(increment));
		time += increment;
		std::this_thread::sleep_for(
				std::chrono::microseconds(static_cast<int>(realTime * increment * 1000)));
	}

}
