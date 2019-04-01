/*
 * structures.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */
#include "structures.h"




std::vector<std::shared_ptr<Node>>
Node::getAncestors()
{
	std::vector<std::shared_ptr<Node>> ancestors;
	for (auto it : prev)
	{
		ancestors.push_back(it);
		ancestors.insert(ancestors.end(), it->getAncestors().begin(), it->getAncestors().end());
	}
	return ancestors;
}

bool
isChain(const std::vector<std::shared_ptr<Node>>& nodes)
{
	for (int k = 0; k < nodes.size() - 1; k++)
	{
		bool temp = false;
		for (auto next : nodes[k]->next)
		{
			if (next == nodes[k+1])
			{
				temp = true;
				break;
			}
		}
		if (!temp)
			return false;
	}

	return true;
}

void
MultiNode::createNodes(unsigned hyperPeriod, std::shared_ptr<Node> globalS, std::shared_ptr<Node> globalE)
{
	unsigned numNodes = hyperPeriod / period;

	for (int k = 0; k < numNodes; k++)
	{
		auto node = std::make_shared<Node>();
		node->wcet = wcet;
		node->offset = k*period;
		node->deadline = k*period + deadline;
		nodes.push_back(node);
	}

	for (int k = 0; k < numNodes; k++)
		{
			auto node = std::make_shared<Node>();
			node->wcet = wcet;
			node->offset = k*period;
			node->deadline = k*period + deadline;
			nodes.push_back(node);
		}
}
