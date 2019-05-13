/*
 * TaskSet.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/TaskSet.h>


const TaskSet::Task&
TaskSet::getTask(unsigned taskId)
{
	return tasks[taskId];
}

