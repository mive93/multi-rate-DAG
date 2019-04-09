/*
 * MultiNode.h
 *
 *  Created on: Apr 8, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_MULTINODE_H_
#define MULTIRATE_MULTINODE_H_
#include <memory>

#include "DAG/Node.h"
#include "HeterogeneousInfo.h"


struct MultiNode
{
//	unsigned offset;
	unsigned deadline;
	unsigned period;
	unsigned wcet;
	unsigned bcet;

	unsigned id;

	std::string name;

	HeterogeneousInfo* info = nullptr;

	std::vector<std::shared_ptr<Node>>
	createNodes(unsigned hyperPeriod);

	std::vector<std::shared_ptr<Node>> nodes;

};


#endif /* MULTIRATE_MULTINODE_H_ */
