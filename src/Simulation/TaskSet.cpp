/*
 * TaskSet.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/CoreManager.h>
#include <Simulation/DAGScheduler.h>
#include <Simulation/TaskSet.h>
#include <uavAP/Core/Logging/APLogger.h>
#include <uavAP/Core/Scheduler/IScheduler.h>
#include <uavAP/Core/Time.h>

TaskSet::TaskSet(const PlainTaskSet& plain)
{
	for (unsigned k = 0; k < plain.bcet.size(); k++)
	{
		tasks_.push_back(Task(plain.wcet[k], plain.bcet[k]));
		tasks_.back().name = plain.name[k];
	}
}

const TaskSet::Function&
TaskSet::getTask(unsigned taskId)
{
	return fakeTasks_[taskId];
}

void
TaskSet::notifyAggregationOnUpdate(const Aggregator& agg)
{
	scheduler_.setFromAggregationIfNotSet(agg);
	dagScheduler_.setFromAggregationIfNotSet(agg);
	coreManager_.setFromAggregationIfNotSet(agg);
}

bool
TaskSet::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!scheduler_.isSet())
		{
			APLOG_ERROR << "Scheduler missing.";
			return true;
		}
		break;
	}
	case RunStage::NORMAL:
	{
		createFakeTasks();
		break;
	}
	default:
		break;
	}
	return false;
}

void
TaskSet::createFakeTasks()
{
	for (unsigned k = 0; k < tasks_.size(); k++)
	{
		fakeTasks_.push_back(std::bind(&TaskSet::fakeTask, this, tasks_[k], k));
	}
}

void
TaskSet::fakeTask(const Task& task, unsigned taskId)
{
	task.readFunction();

	float c = getExectutionTime(taskId);

	auto sched = scheduler_.get();
	sched->schedule(std::bind(&TaskSet::writeAndNotify, this, task.writeFunction, taskId),
			Microseconds(static_cast<int>(c * 1000)));

}

void
TaskSet::writeAndNotify(const Function& writeTask, unsigned taskId)
{
	writeTask();

	APLOG_DEBUG << tasks_[taskId].name << " finished";
	auto dagSched = dagScheduler_.get();
	dagSched->taskFinished(taskId);

	auto coreMan = coreManager_.get();
	coreMan->taskFinished();
}

float
TaskSet::getExectutionTime(unsigned taskId)
{
	APLOG_TRACE << "Get exec time:" << tasks_[taskId].wcet;
	return tasks_[taskId].wcet;
}

void
TaskSet::setReadFunction(unsigned taskId, const Function& readFcn)
{
	tasks_[taskId].readFunction = readFcn;
}

void
TaskSet::setWriteFunction(unsigned taskId, const Function& writeFcn)
{
	tasks_[taskId].writeFunction = writeFcn;
}
