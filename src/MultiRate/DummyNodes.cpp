/*
 * DummyNodes.cpp
 *
 *  Created on: Apr 10, 2019
 *      Author: mirco
 */
#include "MultiRate/DummyNodes.h"

#include <set>
#include <iostream>
#include <map>

void
DummyNodes::addToDAG(DAG& dag, unsigned hyperperiod)
{
	auto dagNodes = dag.getNodes();

	std::set<float> dummyVals;

	for (auto node : dagNodes)
	{
		dummyVals.insert(node->offset);
		dummyVals.insert(node->deadline);
	}

	std::map<float, std::shared_ptr<Node>> sync;

	for (auto it = dummyVals.begin(); it != dummyVals.end(); it++)
	{
		//Skip start and end
		if (*it == 0 || *it == hyperperiod)
			continue;

		auto syncNode = std::make_shared<Node>(*it, *it, 0, 0, 666);
		syncNode->name = "sync" + std::to_string(static_cast<int>(*it));
		syncNode->shortName = std::to_string(static_cast<int>(*it));

		sync.insert(std::make_pair(*it, syncNode));
		syncNodes.push_back(syncNode);
	}

	std::vector<Edge> syncEdges;
	for (auto node : dagNodes)
	{
		auto startSync = sync.find(node->offset);
		if (startSync != sync.end())
			syncEdges.push_back(Edge(startSync->second, node));

		auto endSync = sync.find(node->deadline);
		if (endSync != sync.end())
			syncEdges.push_back(Edge(node, endSync->second));
	}

	if(syncNodes.size() > 0)
	{

		auto dummy = std::make_shared<Node>(0, syncNodes.front()->offset, syncNodes.front()->offset,
				syncNodes.front()->offset, 667);
		dummy->name = "dummy0-" + std::to_string(static_cast<int>(syncNodes.front()->offset));
		dummy->shortName = "0-" + std::to_string(static_cast<int>(syncNodes.front()->offset));
		dummyTasks.push_back(dummy);
		dummyChain.push_back(Edge(dag.getStart(), dummy));
		dummyChain.push_back(Edge(dummy, syncNodes.front()));
		for (auto it = syncNodes.begin(); it != syncNodes.end(); it++)
		{
			auto next = std::next(it);
			if (next == syncNodes.end())
				break;
			float s = (*it)->deadline;
			float e = (*next)->offset;
			auto d = std::make_shared<Node>(s, e, e - s, e - s, 667);
			d->name = "dummy" + std::to_string(static_cast<int>(s)) + "-" + std::to_string(static_cast<int>(e));
			d->shortName = std::to_string(static_cast<int>(s)) + "-" + std::to_string(static_cast<int>(e));

			dummyTasks.push_back(d);
			dummyChain.push_back(Edge(*it, d));
			dummyChain.push_back(Edge(d, *next));
		}
		float s = syncNodes.back()->deadline;
		float e = dag.getEnd()->offset;
		dummy = std::make_shared<Node>(s, e, e - s, e - s, 667);
		dummy->name = "dummy" + std::to_string(static_cast<int>(s)) + "-" + std::to_string(static_cast<int>(e));
		dummy->shortName = std::to_string(static_cast<int>(s)) + "-" + std::to_string(static_cast<int>(e));
		dummyTasks.push_back(dummy);
		dummyChain.push_back(Edge(syncNodes.back(), dummy));
		dummyChain.push_back(Edge(dummy, dag.getEnd()));
	}


	dag.addNodes(syncNodes);
	dag.addNodes(dummyTasks);
	dag.addEdges(dummyChain);
	dag.addEdges(syncEdges);

	//Adding these edges to the chain after adding the chain to the dag to not duplicate edges
	std::cout << "Adding " << syncNodes.size() << " Sync nodes" << std::endl;
	std::cout << "Adding " << dummyTasks.size() << " Dummy tasks" << std::endl;
	std::cout << "Adding " << syncEdges.size() << " sync edges" << std::endl;

}

bool
DummyNodes::brokenDummyChain(const DAG& dag)
{
	for (const auto& edge : dummyChain)
		if (!dag.hasEdge(edge))
			return true;
	return false;
}

