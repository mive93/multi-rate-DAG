/*
 * Task.cpp
 *
 *  Created on: May 18, 2019
 *      Author: mirco
 */
#include <Simulation/Task.h>

Task::Task(float wc, float bc) :
		readFunction(std::bind(&Task::emptyFunc, this)), writeFunction(
				std::bind(&Task::emptyFunc, this)), wcet(wc), bcet(bc)
{
}
