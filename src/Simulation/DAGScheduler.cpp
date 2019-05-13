/*
 * DAGScheduler.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/DAGScheduler.h>

DAGScheduler::DAGScheduler(const PlainDAG& dag, const TaskSet& taskset) :
		dag_(dag), taskSet_(taskset)
{
	depMatrix_.resize(dag_.dagMatrix.rows(), dag_.dagMatrix.cols() + dag_.syncMatrixOffset.cols());
	reset();
}

TaskSet::Task
DAGScheduler::nextTask()
{
}

void
DAGScheduler::taskFinished(unsigned taskId)
{
	depMatrix_.col(taskId).setZero();
	std::cout << depMatrix_ << std::endl << std::endl;
}

void
DAGScheduler::reset()
{
	depMatrix_ << dag_.dagMatrix, dag_.syncMatrixOffset;

	std::cout << depMatrix_ << std::endl << std::endl;
}
