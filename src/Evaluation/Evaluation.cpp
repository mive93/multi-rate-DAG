/*
 * Evaluation.cpp
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */
#include <Evaluation/Evaluation.h>
#include <Evaluation/LatencyInfo.h>
#include <Evaluation/Scheduling.h>
#include <MultiRate/MultiRateTaskset.h>
#include <cmath>
#include <iostream>


void
Evaluation::addLatency(const Chain& chain, const LatencyCost& cost,
		const LatencyConstraint& constraint)
{
	latencyEval_.push_back(std::make_pair(chain, std::make_pair(cost, constraint)));
}

const DAG&
Evaluation::evaluate(const std::vector<DAG>& dags)
{
	std::vector<float> cost(dags.size(), 0.0);

	unsigned invalidDags = 0;
	for (const auto& eval : latencyEval_)
	{
		std::vector<unsigned> chain = taskChainToNum(eval.first);

		for (unsigned k = 0; k < dags.size(); k++)
		{
			if (std::isnan(cost[k]))
				continue;

			auto info = dags[k].getLatencyInfo(chain);

			if (!eval.second.second.isValid(info))
			{
				cost[k] = NAN;
				invalidDags++;
				continue;
			}

			cost[k] += eval.second.first.getCost(info);

		}
	}

	for (unsigned k = 0; k < dags.size(); k++)
	{
		if (std::isnan(cost[k]))
			continue;

		SchedulingInfo info = getSchedulingInfo(dags[k], schedulingEval_.second);

		if (!schedulingEval_.second.isValid(info))
		{
			cost[k] = NAN;
			invalidDags++;
			continue;
		}

		cost[k] += schedulingEval_.first.getCost(info);

	}
	std::cout << "Num invalid dags: " << invalidDags << std::endl;

	if (invalidDags == dags.size())
	{
		std::cout << "No valid dag found. Constraints are too tight." << std::endl;
		return dags[0];
	}

	unsigned bestDAG = 0;
	float minCost = std::numeric_limits<float>::max();
	for (unsigned k = 0; k < cost.size(); k++)
	{
		if (!std::isnan(cost[k]) && cost[k] < minCost)
		{
			bestDAG = k;
			minCost = cost[k];
		}
	}

	std::cout << "Best DAG: " << bestDAG << ", with total cost: " << minCost << std::endl << std::endl;
	for (const auto& eval : latencyEval_)
	{
		printChain(eval.first);
		std::cout << dags[bestDAG].getLatencyInfo(taskChainToNum(eval.first)) << std::endl;
	}


	return dags[bestDAG];
}

void
Evaluation::addScheduling(const SchedulingCost& cost, const SchedulingConstraint& constraint)
{
	schedulingEval_ = std::make_pair(cost, constraint);
}

std::vector<unsigned>
Evaluation::taskChainToNum(const Chain& chain)
{
	std::vector<unsigned> c;
	for (const auto& node : chain)
	{
		c.push_back(node->id - 1);
	}
	return c;
}

void
Evaluation::printChain(const Chain& chain)
{
	std::cout << "Chain: ";

	for (const auto& n : chain)
	{
		std::cout <<"->" << n->name;
	}
	std::cout << std::endl;
}

SchedulingInfo
Evaluation::getSchedulingInfo(const DAG& dag, const SchedulingConstraint& constraint)
{
	float u = dag.getOriginatingTaskset()->getUtilization();

	for (unsigned m = std::ceil(u); m <= constraint.maxCores; m++)
	{
		if (scheduling::scheduleDAG(dag, m))
			return SchedulingInfo(m);
	}

	return SchedulingInfo(constraint.maxCores + 1);
}




































