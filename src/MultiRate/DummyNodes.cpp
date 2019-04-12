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
DummyNodes::addToDAG(DAG& dag)
{
	auto dagNodes = dag.getNodes();

	std::set<unsigned> dummyVals;

	for (auto node : dagNodes)
	{
		dummyVals.insert(node->offset);
		dummyVals.insert(node->deadline);
	}

	std::map<unsigned, std::shared_ptr<Node>> dummyEnd;
	std::map<unsigned, std::shared_ptr<Node>> dummyStart;
	for (auto it = dummyVals.begin(); it != dummyVals.end(); it++)
	{
		auto dummyNode = std::make_shared<Node>();
		int begin = *it;
		auto next = std::next(it);
		if (next == dummyVals.end())
			break;
		int end = *next;
		dummyNode->offset = begin;
		dummyNode->deadline = end;
		dummyNode->wcet = end - begin;
		dummyNode->bcet = end - begin;
		dummyNode->name = "dummy" + std::to_string(begin) + "-" + std::to_string(end);
		dummyNode->shortName = std::to_string(begin) + "-" + std::to_string(end);
		dummyNode->groupId = 666;
		nodes.push_back(dummyNode);
		dummyStart.insert(std::make_pair(begin, dummyNode));
		dummyEnd.insert(std::make_pair(end, dummyNode));
	}


	std::vector<Edge> syncEdges;
	for (auto node : dagNodes)
	{
		auto startSync = dummyEnd.find(node->offset);
		if (startSync != dummyEnd.end())
			syncEdges.push_back(Edge(startSync->second, node));

		auto endSync = dummyStart.find(node->deadline);
		if (endSync != dummyStart.end())
			syncEdges.push_back(Edge(node, endSync->second));
	}

	//Edges from start to dummy and dummy to end already there
	for (auto it = nodes.begin(); it != nodes.end(); it++)
	{
		auto next = std::next(it);
		if (next == nodes.end())
			break;
		dummyChain.push_back(Edge(*it, *next));
	}

	dag.addNodes(nodes);
	dag.addEdges(dummyChain);
	dag.addEdges(syncEdges);

	//Adding these edges to the chain after adding the chain to the dag to not duplicate edges
	dummyChain.insert(dummyChain.begin(), Edge(dag.getStart(), nodes.front()));
	dummyChain.push_back(Edge(nodes.back(), dag.getEnd()));

	std::cout << "Adding " << nodes.size() << " Dummy nodes" << std::endl;
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










