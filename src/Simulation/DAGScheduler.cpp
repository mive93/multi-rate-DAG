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
	depMatrix_.resize(numNodes_, numNodes_ + dag_.syncMatrixOffset.cols() + 3);
}

int
DAGScheduler::nextTask()
{
	if (ready_.empty())
		return -1;

	int ready = ready_.begin()->second;
	ready_.erase(ready_.begin());

	depMatrix_.coeffRef(ready, depMatrix_.cols() - 2) = 1; //Set job as started

	return taskFromJob(ready);
}

void
DAGScheduler::taskFinished(unsigned taskId)
{
	auto jobs = jobsFromTask(taskId);

	for (auto job : jobs)
		if (depMatrix_.coeff(job, depMatrix_.cols() - 2) && !depMatrix_.coeff(job, depMatrix_.cols() - 1))
		{
			depMatrix_.col(job).setZero();
			depMatrix_.coeffRef(job, depMatrix_.cols() - 1) = 1;
			break;
		}
	queueReady();
}

void
DAGScheduler::reset()
{
	APLOG_DEBUG << "DAGScheduler reset";

	if (!depMatrix_.col(depMatrix_.cols() - 1).isOnes())
	{
		APLOG_ERROR << "DEADLINE MISSED!!!!!!     "
				<< depMatrix_.col(depMatrix_.cols() - 1).transpose();
	}

	initDepMatrix();

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
			depMatrix_.coeffRef(k, depMatrix_.cols() - 3) = 1;
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
		scheduler->schedule(std::bind(&DAGScheduler::syncReady, this, k),
				Microseconds(dag_.syncTimes[k] * 1000));
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
		initDepMatrix();
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
	APLOG_TRACE << "Sync ready: " << syncId;
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

void
DAGScheduler::initDepMatrix()
{
	depMatrix_ << dag_.dagMatrix, dag_.syncMatrixOffset, BoolMatrix::Zero(depMatrix_.rows(), 3);

	ready_.clear();
	queueReady();
	scheduleSync();
}

int
DAGScheduler::taskFromJob(int job)
{
	for (int k = 0; k < dag_.groupMatrix.cols(); k++)
		if (dag_.groupMatrix.coeff(job, k))
			return k;

	return -1;
}

std::vector<int>
DAGScheduler::jobsFromTask(int task)
{
	std::vector<int> jobs;
	for (int k = 0; k < dag_.groupMatrix.rows(); k++)
		if (dag_.groupMatrix.coeff(k, task))
			jobs.push_back(k);
	return jobs;
}
