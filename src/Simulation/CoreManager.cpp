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

CoreManager::CoreManager() :
		totalCores_(0), availableCores_(0)
{
}

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
CoreManager::taskReady()
{
	availableCores_++;
	if (availableCores_ > totalCores_)
		availableCores_ = totalCores_;

	getTask();
}

void
CoreManager::getTask()
{
	auto dagSched = dagScheduler_.get();

	int next = dagSched->nextTask();
	if (next == -1)
		return;

	if (availableCores_ <= 0)
	{
		return;
	}
	const auto& info = dagSched->getNodeInfo();
	auto sched = scheduler_.get();
	sched->schedule(std::bind(&CoreManager::taskFinished, this, static_cast<unsigned>(next)), Microseconds(1000 * info.wc[next]));
	availableCores_--;
}

void
CoreManager::taskFinished(unsigned taskId)
{
	auto dagSched = dagScheduler_.get();
	dagSched->taskFinished(taskId);
	taskReady();
}
