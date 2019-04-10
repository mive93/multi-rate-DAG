/*
 * MultiNode.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mirco
 */
#include "MultiRate/MultiNode.h"

std::vector<std::shared_ptr<Node>>
MultiNode::createNodes(unsigned hyperPeriod)
{
	unsigned numNodes = hyperPeriod / period;

	for (int k = 0; k < numNodes; k++)
	{
		auto node = std::make_shared<Node>();
		node->wcet = wcet;
		node->bcet = bcet;
		node->groupId = id;
		node->offset = k * period;
		node->deadline = k * period + deadline;
		node->name = name + "-" + std::to_string(k);
		node->shortName = std::to_string(id) + "-" + std::to_string(k);
		nodes.push_back(node);
	}

	return nodes;
}
