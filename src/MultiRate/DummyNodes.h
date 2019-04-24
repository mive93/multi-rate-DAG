/*
 * DummyNodes.h
 *
 *  Created on: Apr 10, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_DUMMYNODES_H_
#define MULTIRATE_DUMMYNODES_H_
#include <memory>
#include <vector>

#include "DAG/DAG.h"
#include "DAG/Node.h"

struct DummyNodes
{

	void
	addToDAG(DAG& dag, unsigned hyperperiod);

	bool
	brokenDummyChain(const DAG& dag);

	std::vector<std::shared_ptr<Node>> dummyTasks;
	std::vector<std::shared_ptr<Node>> syncNodes;
	std::vector<Edge> dummyChain;
};


#endif /* MULTIRATE_DUMMYNODES_H_ */
