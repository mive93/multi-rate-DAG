/*
 * TaskSet.h
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_TASKSET_H_
#define SIMULATION_TASKSET_H_
#include <Simulation/Task.h>
#include <cpsCore/Aggregation/AggregatableObject.hpp>
#include <cpsCore/Synchronization/IRunnableObject.h>
#include <VariableTaskSet/PlainTaskSet.h>
#include <vector>
#include <functional>

class IScheduler;
class DAGScheduler;
class CoreManager;

class TaskSet : public AggregatableObject<DAGScheduler, IScheduler, CoreManager>, public IRunnableObject
{

public:

	static constexpr TypeId typeId = "Taskset";

	TaskSet();

	TaskSet(const PlainTaskSet& plain);

	using Function = std::function<void()>;

	bool
	run(RunStage stage) override;

	void
	setReadFunction(unsigned taskId, const Function& readFcn);

	void
	setWriteFunction(unsigned taskId, const Function& writeFcn);

	const Function&
	getTask(unsigned taskId);

	unsigned
	getNumTasks() const;

	void
	setSeed(unsigned seed);

private:

	void
	createFakeTasks();

	void
	fakeTask(const Task& task, unsigned taskId);

	void
	writeAndNotify(const Function& writeTask, unsigned taskId);

	float
	getExectutionTime(unsigned taskId);

	unsigned seed_;

	std::vector<Task> tasks_;

	std::vector<Function> fakeTasks_;

};



#endif /* SIMULATION_TASKSET_H_ */
