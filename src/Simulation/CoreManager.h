/*
 * CoreManager.h
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_COREMANAGER_H_
#define SIMULATION_COREMANAGER_H_
#include <uavAP/Core/Object/IAggregatableObject.h>
#include <uavAP/Core/Object/ObjectHandle.h>
#include <uavAP/Core/Runner/IRunnableObject.h>

class DAGScheduler;
class IScheduler;
class TaskSet;


class CoreManager : public IAggregatableObject, public IRunnableObject
{
public:

	CoreManager(unsigned cores);

	void
	setNumCores(unsigned numCores);

	void
	notifyAggregationOnUpdate(const Aggregator& agg) override;

	bool
	run(RunStage stage) override;

	void
	syncReady();

	void
	taskFinished();

private:

	void
	getTask();

	unsigned totalCores_;
	unsigned availableCores_;

	ObjectHandle<DAGScheduler> dagScheduler_;
	ObjectHandle<IScheduler> scheduler_;
	ObjectHandle<TaskSet> taskSet_;


};



#endif /* SIMULATION_COREMANAGER_H_ */
