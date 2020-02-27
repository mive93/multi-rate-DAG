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

void Evaluation::addLatency(const std::vector<std::shared_ptr<MultiNode>> &chain,
							const LatencyCost &cost, const LatencyConstraint &constraint)
{
	latencyEval_.push_back(std::make_pair(taskChainToNum(chain), std::make_pair(cost, constraint)));
}

void Evaluation::addChain(const std::vector<unsigned> &chain, const LatencyCost &cost,
						  const LatencyConstraint &constraint)
{
	latencyEval_.push_back(std::make_pair(chain, std::make_pair(cost, constraint)));
}

const DAG &
Evaluation::evaluate(const std::vector<DAG> &dags, DataFiles *f)
{
	std::vector<float> cost(dags.size(), 0.0);

	unsigned invalidDags = 0;
	int k = 0;
	for (const auto &dag : dags)
	{
		// std::cout << "Evaluating DAG " << k << "/" <<dags.size() << " ";
		cost[k] = evaluate(dag);
		if (std::isnan(cost[k++]))
		{
			invalidDags++;
			// std::cout << "invalid" << std::endl;
		}
		// else
		// std::cout << "cost: " << cost[k-1] << std::endl;
	}

	std::cout << "Num invalid dags: " << invalidDags << std::endl;
	if (f)
		f->sd << dags.size() - invalidDags << ";";

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

	std::cout << "Best DAG: " << bestDAG << ", with total cost: " << minCost << std::endl
			  << std::endl;
	for (const auto &eval : latencyEval_)
	{
		printChain(eval.first);
		// for (const auto &d : dags)
		// {
		// 	auto latInfo = d.getLatencyInfoNoCutoff(eval.first);
		// 	std::cout << latInfo << std::endl;
		// }

		auto latInfo = dags[bestDAG].getLatencyInfoNoCutoff(eval.first);
		std::cout << latInfo << std::endl;
		if (f)
		{
			f->addR(latInfo.reactionTime);
			f->addDA(latInfo.maxLatency);
		}
	}

	std::cout << "Cores needed: "
			  << getSchedulingInfo(dags[bestDAG], schedulingEval_.second).numCoresNeeded << std::endl;

	return dags[bestDAG];
}

float Evaluation::evaluate(const DAG &dag)
{
	float cost = 0;
	std::vector<LatencyInfo> latencies;
	for (const auto &eval : latencyEval_)
	{
		auto info = dag.getLatencyInfoNoCutoff(eval.first);

		latencies.push_back(info);

		if (!eval.second.second.isValid(info))
		{
			return NAN;
		}

		cost += eval.second.first.getCost(info);
	}

	SchedulingInfo info = getSchedulingInfo(dag, schedulingEval_.second);

	if (!schedulingEval_.second.isValid(info))
	{
		return NAN;
	}

	latencies_.push_back(latencies);
	cost += schedulingEval_.first.getCost(info);
	return cost;
}

void Evaluation::addScheduling(const SchedulingCost &cost, const SchedulingConstraint &constraint)
{
	schedulingEval_ = std::make_pair(cost, constraint);
}

std::vector<unsigned>
Evaluation::taskChainToNum(const std::vector<std::shared_ptr<MultiNode>> &chain)
{
	std::vector<unsigned> c;
	for (const auto &node : chain)
	{
		c.push_back(node->id - 1);
	}
	return c;
}

void Evaluation::printChain(const std::vector<unsigned> &chain) const
{
	std::cout << "Chain: ";

	for (const auto &n : chain)
	{
		std::cout << "->" << n;
	}
	std::cout << std::endl;
}

SchedulingInfo
Evaluation::getSchedulingInfo(const DAG &dag, const SchedulingConstraint &constraint)
{
	float u = dag.getOriginatingTaskset()->getUtilization();

	for (unsigned m = std::ceil(u); m <= constraint.maxCores; m++)
	{
		if (scheduling::scheduleDAG(dag, m))
			return SchedulingInfo(m);
	}

	return SchedulingInfo(constraint.maxCores + 1);
}

