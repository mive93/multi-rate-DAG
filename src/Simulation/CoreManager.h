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

class CoreManager : public IAggregatableObject, public IRunnableObject
{
public:

	CoreManager();

	void
	setNumCores(unsigned numCores);

	void
	notifyAggregationOnUpdate(const Aggregator& agg) override;

	bool
	run(RunStage stage) override;

	void
	syncReady();

	void
	taskReady();

private:

	void
	getTask();

	void
	taskFinished(unsigned taskId);

	unsigned totalCores_;
	unsigned availableCores_;

	ObjectHandle<DAGScheduler> dagScheduler_;
	ObjectHandle<IScheduler> scheduler_;


};



#endif /* SIMULATION_COREMANAGER_H_ */
