/*
 * CoreManager.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/CoreManager.h>
#include <Simulation/DAGScheduler.h>
#include <cpsCore/Logging/CPSLogger.h>
#include <cpsCore/Utilities/Scheduler/IScheduler.h>
#include <cpsCore/Utilities/Time.hpp>

void
CoreManager::setNumCores(unsigned numCores)
{
	totalCores_ = numCores;
}

bool
CoreManager::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!checkIsSetAll())
		{
			CPSLOG_ERROR << "DAGScheduler missing.";
			return true;
		}
		break;
	}
	case RunStage::NORMAL:
	{
		auto sched = get<IScheduler>();
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

	auto dagSched = get<DAGScheduler>();

	int next = dagSched->nextTask();
	if (next == -1)
		return;

	auto taskSet = get<TaskSet>();

	auto task = taskSet->getTask(next);

	auto sched = get<IScheduler>();
	sched->schedule(task, Microseconds(0));
	availableCores_--;
	if (availableCores_ > 0)
		getTask();
}

CoreManager::CoreManager(unsigned cores) :
		totalCores_(cores), availableCores_(cores)
{
}
