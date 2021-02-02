/*
 * DAGScheduler.h
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_DAGSCHEDULER_H_
#define SIMULATION_DAGSCHEDULER_H_
#include <DAG/PlainDAG.h>
#include <Simulation/TaskSet.h>
#include <cpsCore/Aggregation/IAggregatableObject.h>
#include <cpsCore/Aggregation/ObjectHandleContainer.hpp>
#include <cpsCore/Synchronization/IRunnableObject.h>

class IScheduler;
class CoreManager;

class DAGScheduler: public IAggregatableObject, public IRunnableObject
{
public:

	using BoolMatrix = PlainDAG::BoolMatrix;

	DAGScheduler(const PlainDAG& dag);

	int
	nextTask();

	void
	taskFinished(unsigned taskId);

	void
	reset();

	void
	initDepMatrix();

	void
	notifyAggregationOnUpdate(const Aggregator& agg) override;

	bool
	run(RunStage stage) override;

	const DAG::NodeInfo&
	getNodeInfo() const;

	const BoolMatrix&
	getDepMatrix() const
	{
		return depMatrix_;
	}

private:

	void
	queueReady();

	void
	scheduleSync();

	void
	syncReady(unsigned syncId);

	int
	taskFromJob(int job);

	std::vector<int>
	jobsFromTask(int task);

	PlainDAG dag_;
	BoolMatrix depMatrix_;
	BoolMatrix deadlineMatrix_;

	std::multimap<float, int> ready_;
	unsigned numNodes_;

	ObjectHandleContainer<IScheduler> scheduler_;
	ObjectHandleContainer<CoreManager> coreManager_;

};

#endif /* SIMULATION_DAGSCHEDULER_H_ */
