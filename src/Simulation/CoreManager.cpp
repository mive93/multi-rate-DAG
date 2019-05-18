/*
 * CoreManager.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/CoreManager.h>
#include <Simulation/DAGScheduler.h>
#include <uavAP/Core/Logging/APLogger.h>
#include <uavAP/Core/Scheduler/IScheduler.h>
#include <uavAP/Core/Time.h>

void
CoreManager::setNumCores(unsigned numCores)
{
	totalCores_ = numCores;
}

void
CoreManager::notifyAggregationOnUpdate(const Aggregator& agg)
{
	dagScheduler_.setFromAggregationIfNotSet(agg);
	scheduler_.setFromAggregationIfNotSet(agg);
	taskSet_.setFromAggregationIfNotSet(agg);
}

bool
CoreManager::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!dagScheduler_.isSet())
		{
			APLOG_ERROR << "DAGScheduler missing.";
			return true;
		}
		break;
	}
	case RunStage::NORMAL:
	{
		auto sched = scheduler_.get();
		sched->schedule(std::bind(&CoreManager::getTask, this), Microseconds(0));
		availableCores_ = totalCores_;
		break;
	}
	default:
		break;
	}
	return false;
}

void
CoreManager::syncReady()
{
	getTask();
}

void
CoreManager::taskFinished()
{
	availableCores_++;
	if (availableCores_ > totalCores_)
		availableCores_ = totalCores_;

	getTask();
}

void
CoreManager::getTask()
{

	if (availableCores_ <= 0)
	{
		return;
	}

	auto dagSched = dagScheduler_.get();

	int next = dagSched->nextTask();
	if (next == -1)
		return;

	auto taskSet = taskSet_.get();

	auto task = taskSet->getTask(next);

	auto sched = scheduler_.get();
	sched->schedule(task, Microseconds(0));
	availableCores_--;
	if (availableCores_ > 0)
		getTask();
}

CoreManager::CoreManager(unsigned cores) :
		totalCores_(cores), availableCores_(cores)
{
}
