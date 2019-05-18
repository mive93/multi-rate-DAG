/*
 * Task.h
 *
 *  Created on: May 18, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_TASK_H_
#define SIMULATION_TASK_H_

#include <functional>

struct Task
{
	Task(float wc, float bc);

	using Function = std::function<void()>;

	Function readFunction;
	Function writeFunction;

	float wcet;
	float bcet;

	std::string name;

	void emptyFunc(){}

};

#endif /* SIMULATION_TASK_H_ */
