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



class DAGScheduler
{
public:

	DAGScheduler(const PlainDAG& dag, const TaskSet& taskset);

	TaskSet::Task
	nextTask();

	void
	taskFinished(unsigned taskId);

	void
	reset();

private:

	PlainDAG dag_;
	TaskSet taskSet_;

	Eigen::Matrix<bool, -1, -1> depMatrix_;


};


#endif /* SIMULATION_DAGSCHEDULER_H_ */
