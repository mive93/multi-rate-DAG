/*
 * MultiRate.cpp
 *
 *  Created on: Apr 3, 2019
 *      Author: mirco
 */
#include "MultiRate/MultiRateTaskset.h"
#include "MultiRate/MultiNode.h"

#include <vector>
MultiRateTaskset::MultiRateTaskset() :
		hyperPeriod_(0)
{
}

std::shared_ptr<MultiNode>
MultiRateTaskset::addTask(unsigned period, unsigned wcet, unsigned deadline,
		const std::string& name)
{
	auto mult = std::make_shared<MultiNode>();
	mult->period = period;
	mult->wcet = wcet;
	mult->deadline = deadline;
	mult->id = nodes_.size();

	if (name.empty())
		mult->name = std::to_string(mult->id);
	else
		mult->name = name;

	nodes_.push_back(mult);

	if (period > hyperPeriod_)
		hyperPeriod_ = period;

	return mult;
}

std::shared_ptr<MultiNode>
MultiRateTaskset::addTask(unsigned period, unsigned wcet, const std::string& name)
{
	return addTask(period, wcet, period, name);
}

std::shared_ptr<MultiEdge>
MultiRateTaskset::addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to)
{
	auto edge = std::make_shared<MultiEdge>();
	edge->from = from;
	edge->to = to;
	edge->dependency = MultiEdge::Dependency::PRECEDENCE;
	edge->jitter = 0; //Precedence does not have jitter
	edges_.push_back(edge);

	return edge;
}

std::shared_ptr<MultiEdge>
MultiRateTaskset::addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to,
		unsigned jitter)
{
	auto edge = std::make_shared<MultiEdge>();
	edge->from = from;
	edge->to = to;
	edge->dependency = MultiEdge::Dependency::DATA;
	edge->jitter = jitter;
	edges_.push_back(edge);

	return edge;
}

const DAG&
MultiRateTaskset::createBaselineDAG()
{
	baselineDAG_.setPeriod(hyperPeriod_);
	auto start = baselineDAG_.getStart();
	auto end = baselineDAG_.getEnd();
	for (auto& node : nodes_)
	{
		auto nodes = node->createNodes(hyperPeriod_);
		std::vector<std::shared_ptr<Edge>> edges;
		edges.push_back(std::make_shared<Edge>(start, nodes.front()));
		for (unsigned k = 0; k < nodes.size() - 1; k++)
		{
			edges.push_back(std::make_shared<Edge>(nodes[k], nodes[k + 1]));
		}
		edges.push_back(std::make_shared<Edge>(nodes.back(), end));
		baselineDAG_.addNodes(nodes);
		baselineDAG_.addEdges(edges);
	}
	return baselineDAG_;
}

std::vector<DAG>
MultiRateTaskset::createDAGs()
{
	std::vector<std::vector<std::vector<Edge>>> edgeSets;
	auto start = baselineDAG_.getStart();
	auto end = baselineDAG_.getEnd();
	for (auto edge : edges_)
	{
		std::vector<std::vector<Edge>> edgeSet;
		unsigned i = edge->from->nodes.size();
		unsigned j = edge->to->nodes.size();

		for (unsigned k = 0; k < i; k++)
		{

		}

		if (edge->dependency == MultiEdge::Dependency::PRECEDENCE)
		{

		}



		edgeSets.push_back(edgeSet);
	}

	std::vector<int> permutation(edgeSets.size(), 0);
	int numPermutations = 1;
	for (const auto& it : edgeSets)
		numPermutations *= it.size();

	for (int k = 0; k < numPermutations; k++)
	{

	}

	for (const auto& set : edgeSets)
	{

	}
	return std::vector<DAG>();
}
















