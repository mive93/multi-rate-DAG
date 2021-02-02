/*
 * CoreManager.h
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_COREMANAGER_H_
#define SIMULATION_COREMANAGER_H_
#include <cpsCore/Aggregation/AggregatableObject.hpp>
#include <cpsCore/Synchronization/IRunnableObject.h>

class DAGScheduler;
class IScheduler;
class TaskSet;


class CoreManager : public AggregatableObject<DAGScheduler, IScheduler, TaskSet>, public IRunnableObject
{
public:

	static constexpr TypeId typeId = "CoreManager";

	CoreManager(unsigned cores);

	void
	setNumCores(unsigned numCores);

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

};



#endif /* SIMULATION_COREMANAGER_H_ */
