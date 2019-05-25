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
#include <uavAP/Core/Logging/APLogger.h>
#include <Simulation/TaskSet.h>
#include <cmath>
#include <iostream>

void
Evaluation::addLatency(const std::vector<std::shared_ptr<MultiNode>>& chain,
		const LatencyCost& cost, const LatencyConstraint& constraint)
{
	latencyEval_.push_back(std::make_pair(taskChainToNum(chain), std::make_pair(cost, constraint)));
}

void
Evaluation::addChain(const std::vector<unsigned>& chain, const LatencyCost& cost,
		const LatencyConstraint& constraint)
{
	latencyEval_.push_back(std::make_pair(chain, std::make_pair(cost, constraint)));
}

const DAG&
Evaluation::evaluate(const std::vector<DAG>& dags)
{
	std::vector<float> cost(dags.size(), 0.0);

	unsigned invalidDags = 0;
	int k = 0;
	for (const auto& dag : dags)
	{
		cost[k] = evaluate(dag);
		if (std::isnan(cost[k++]))
			invalidDags++;
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

	std::cout << "Best DAG: " << bestDAG << ", with total cost: " << minCost << std::endl
			<< std::endl;
	for (const auto& eval : latencyEval_)
	{
		printChain(eval.first);
		std::cout << dags[bestDAG].getLatencyInfo(eval.first) << std::endl;
	}

	std::cout << "Cores needed: "
			<< getSchedulingInfo(dags[bestDAG], schedulingEval_.second).numCoresNeeded << std::endl;

	return dags[bestDAG];
}

float
Evaluation::evaluate(const DAG& dag)
{
	float cost = 0;
	std::vector<LatencyInfo> latencies;
	for (const auto& eval : latencyEval_)
	{
		auto info = dag.getLatencyInfo(eval.first);

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

void
Evaluation::addScheduling(const SchedulingCost& cost, const SchedulingConstraint& constraint)
{
	schedulingEval_ = std::make_pair(cost, constraint);
}

std::vector<unsigned>
Evaluation::taskChainToNum(const std::vector<std::shared_ptr<MultiNode>>& chain)
{
	std::vector<unsigned> c;
	for (const auto& node : chain)
	{
		c.push_back(node->id - 1);
	}
	return c;
}

void
Evaluation::printChain(const std::vector<unsigned>& chain) const
{
	std::cout << "Chain: ";

	for (const auto& n : chain)
	{
		std::cout << "->" << n;
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

void
Evaluation::printInfo() const
{
	for (const auto& eval : latencyEval_)
	{
		printChain(eval.first);
		std::cout << eval.second.first << std::endl;
		std::cout << eval.second.second << std::endl;
	}

	for (const auto& chain : chainSims_)
	{
		std::cout << "Data Ages" << std::endl;
		for (const auto& age : chain.ages)
			std::cout << age.total_microseconds() << std::endl;
		std::cout << "Reaction time" << std::endl;
		for (const auto& react : chain.reactions)
			std::cout << react.total_microseconds() << std::endl;
	}
}

void
Evaluation::notifyAggregationOnUpdate(const Aggregator& agg)
{
	taskSet_.setFromAggregationIfNotSet(agg);
	timeProvider_.setFromAggregationIfNotSet(agg);
}

bool
Evaluation::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!taskSet_.isSet())
		{
			APLOG_ERROR << "Taskset missing.";
			return true;
		}
		if (!timeProvider_.isSet())
		{
			APLOG_ERROR << "Time Provider missing.";
			return true;
		}
		break;
	}
	case RunStage::NORMAL:
	{
		initChainSims();
		auto set = taskSet_.get();
		for (unsigned k = 0; k < set->getNumTasks(); k++)
		{
			set->setReadFunction(k, std::bind(&Evaluation::readTask, this, k));
			set->setWriteFunction(k, std::bind(&Evaluation::writeTask, this, k));
		}
		std::cout << "Chain sim num: " << chainSims_.size() << std::endl;
		break;
	}
	default:
		break;
	}
	return false;
}

void
Evaluation::initChainSims()
{
	for (const auto& lat : latencyEval_)
	{
		ChainSim sim;
		sim.chain = lat.first;
		chainSims_.push_back(sim);
	}
}

void
Evaluation::readTask(unsigned task)
{
	auto tp = timeProvider_.get();
	auto time = tp->now();
	for (auto& sim : chainSims_)
		sim.read(task, time);
}

void
Evaluation::exportReactionTimes(const std::string& filename)
{
	std::ofstream file(filename);
	for (const auto& reacts : latencies_)
	{
		for (const auto& time : reacts)
		{
			file << time.reactionTime << " ";
		}
		file << std::endl;
	}
}

void
Evaluation::exportDataAges(const std::string& filename)
{
	std::ofstream file(filename);
	for (const auto& ages : latencies_)
	{
		for (const auto& time : ages)
		{
			file << time.maxLatency << " ";
		}
		file << std::endl;
	}
}

void
Evaluation::writeTask(unsigned task)
{
	auto tp = timeProvider_.get();
	auto time = tp->now();
	for (auto& sim : chainSims_)
		sim.write(task, time);
}

void
Evaluation::exportLatency(const std::string& fileOffset)
{
	unsigned k = 0;
	for (const auto& chain : chainSims_)
	{
		std::ofstream ageFile(fileOffset + "_chain" + std::to_string(k) + "_age");
		for (const auto& age : chain.ages)
			ageFile << age.total_microseconds() << std::endl;
		std::ofstream reactFile(fileOffset + "_chain" + std::to_string(k) + "_react");
		for (const auto& react : chain.reactions)
			reactFile << react.total_microseconds() << std::endl;

		k++;
	}
}
