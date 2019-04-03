/*
 * MultiRate.cpp
 *
 *  Created on: Apr 3, 2019
 *      Author: mirco
 */
#include "MultiRate.h"




MultiRateTaskset::MultiRateTaskset()
{
}

std::shared_ptr<MultiNode>
MultiRateTaskset::addTask(unsigned period, unsigned wcet, unsigned deadline)
{
	auto mult = std::make_shared<MultiNode>();
	mult->period = period;
	mult->wcet = wcet;
	mult->deadline = deadline;
	mult->id = nodes_.size();
	nodes_.push_back(mult);

	return mult;
}

std::shared_ptr<MultiNode>
MultiRateTaskset::addTask(unsigned period, unsigned wcet)
{
	return addTask(period, wcet, period);
}

std::shared_ptr<MultiEdge>
MultiRateTaskset::addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to)
{
	auto edge = std::make_shared<MultiEdge>();
	edge->from = from;
	edge->to = to;
	edge->jitter = -1; // Precedence
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
	edge->jitter = static_cast<int>(jitter);
	edges_.push_back(edge);

	return edge;
}
