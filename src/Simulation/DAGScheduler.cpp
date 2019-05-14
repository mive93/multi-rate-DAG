/*
 * DAGScheduler.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/CoreManager.h>
#include <Simulation/DAGScheduler.h>
#include <uavAP/Core/Logging/APLogger.h>
#include <uavAP/Core/Scheduler/IScheduler.h>
#include <uavAP/Core/Time.h>
#include <set>

DAGScheduler::DAGScheduler(const PlainDAG& dag) :
		dag_(dag)
{
	numNodes_ = dag_.dagMatrix.rows();
	depMatrix_.resize(numNodes_, numNodes_ + dag_.syncMatrixOffset.cols() + 2);
}

int
DAGScheduler::nextTask()
{
	if (ready_.empty())
		return -1;

	int ready = ready_.begin()->second;
	ready_.erase(ready_.begin());
	return ready;
}

void
DAGScheduler::taskFinished(unsigned taskId)
{
	APLOG_DEBUG << "Task " << taskId << " finished";

	depMatrix_.col(taskId).setZero();
	depMatrix_.coeffRef(taskId, depMatrix_.cols() - 1) = 1;
	queueReady();
}

void
DAGScheduler::reset()
{
	APLOG_DEBUG << "DAGScheduler reset";

	if (!depMatrix_.col(depMatrix_.cols() - 1).isOnes())
	{
		APLOG_ERROR << "DEADLINE MISSED!!!!!!     " << depMatrix_.col(depMatrix_.cols() - 1).transpose();
	}
	depMatrix_ << dag_.dagMatrix, dag_.syncMatrixOffset, BoolMatrix::Zero(depMatrix_.rows(), 2);

	ready_.clear();
	queueReady();
	scheduleSync();

	auto coreMan = coreManager_.get();
	coreMan->syncReady();
}

void
DAGScheduler::queueReady()
{
	for (unsigned k = 0; k < depMatrix_.rows(); k++)
	{
		if (depMatrix_.row(k).isZero())
		{
			ready_.insert(std::make_pair(dag_.nodeInfo.lft[k], k));
			depMatrix_.coeffRef(k, depMatrix_.cols() - 2) = 1;
		}
	}

}

void
DAGScheduler::scheduleSync()
{
	auto scheduler = scheduler_.get();
	if (!scheduler)
	{
		APLOG_ERROR << "Scheduler missing. Cannot schedule sync nodes.";
		return;
	}

	for (unsigned k = 0; k < dag_.syncTimes.size(); k++)
	{
		scheduler->schedule(std::bind(&DAGScheduler::syncReady, this, k), Microseconds(dag_.syncTimes[k] * 1000));
	}

	scheduler->schedule(std::bind(&DAGScheduler::reset, this), Microseconds(dag_.period * 1000));
}

bool
DAGScheduler::run(RunStage stage)
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
		reset();
		break;
	}
	default:
		break;
	}
	return false;
}

void
DAGScheduler::syncReady(unsigned syncId)
{
	APLOG_DEBUG << "Sync ready: " << syncId;
	depMatrix_.col(numNodes_ + syncId).setZero();
	queueReady();
	auto coreMan = coreManager_.get();
	coreMan->syncReady();
}

void
DAGScheduler::notifyAggregationOnUpdate(const Aggregator& agg)
{
	scheduler_.setFromAggregationIfNotSet(agg);
	coreManager_.setFromAggregationIfNotSet(agg);
}

const DAG::NodeInfo&
DAGScheduler::getNodeInfo() const
{
	return dag_.nodeInfo;
}
