/*
 * TaskSet.h
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_TASKSET_H_
#define SIMULATION_TASKSET_H_
#include <vector>
#include <functional>

struct TaskSet
{

	using Task = std::function<void()>;

	std::vector<Task> tasks;

	const Task&
	getTask(unsigned taskId);
};



#endif /* SIMULATION_TASKSET_H_ */
