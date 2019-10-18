/*
 * VariableTaskSet.cpp
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */
#include <VariableTaskSet/VariableTaskSet.h>
#include <iostream>

std::shared_ptr<MultiNode>
VariableTaskSet::addTask(unsigned period, float wcet, float deadline, const std::string &name)
{
	return baselineTaskset_.addTask(period, wcet, deadline, name);
}

std::shared_ptr<MultiNode>
VariableTaskSet::addTask(unsigned period, float wcet, const std::string &name)
{
	return baselineTaskset_.addTask(period, wcet, name);
}

const MultiEdge &
VariableTaskSet::addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to)
{
	return baselineTaskset_.addPrecedenceEdge(from, to);
}

const VariableMultiEdge &
VariableTaskSet::addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to,
							 std::vector<unsigned> jitters)
{
	VariableMultiEdge edge;
	edge.from = from;
	edge.to = to;
	edge.jitter = jitters;
	edges_.push_back(edge);
	return edges_.back();
}

MultiRateTaskset &
VariableTaskSet::createBaselineTaskset()
{
	baselineTaskset_.createBaselineDAG();
	return baselineTaskset_;
}

int VariableTaskSet::computePermutations()
{
	std::vector<std::vector<MultiEdge>> edgeSets;

	std::vector<int> permutSets;
	for (auto &edge : edges_)
	{
		edgeSets.push_back(edge.translateToMultiEdges());
		permutSets.push_back(edgeSets.back().size());
	}
	permutSets.push_back(1);

	std::vector<int> permutation(edgeSets.size(), 0);
	int numPermutations = 1;
	for (const auto &it : edgeSets)
		numPermutations *= it.size();

	for (int k = permutSets.size() - 2; k >= 0; k--)
	{
		permutSets[k] = permutSets[k + 1] * permutSets[k];
	}
	std::cout << numPermutations << " Permutations available" << std::endl;
	return numPermutations;
}

const std::vector<MultiRateTaskset> &
VariableTaskSet::createTasksets()
{
	std::vector<std::vector<MultiEdge>> edgeSets;

	std::vector<int> permutSets;
	for (auto &edge : edges_)
	{
		edgeSets.push_back(edge.translateToMultiEdges());
		permutSets.push_back(edgeSets.back().size());
	}
	permutSets.push_back(1);

	std::vector<int> permutation(edgeSets.size(), 0);
	int numPermutations = 1;
	for (const auto &it : edgeSets)
		numPermutations *= it.size();

	for (int k = permutSets.size() - 2; k >= 0; k--)
	{
		permutSets[k] = permutSets[k + 1] * permutSets[k];
	}

	std::cout << numPermutations << " Permutations available" << std::endl;

	tasksets_.clear();
	for (int k = 0; k < numPermutations; k++)
	{
		MultiRateTaskset set(baselineTaskset_);

		int tmp = k;
		for (unsigned i = 0; i < permutation.size(); i++)
		{
			permutation[i] = tmp / permutSets[i + 1];
			tmp = tmp % permutSets[i + 1];
		}

		for (unsigned n = 0; n < edgeSets.size(); n++)
		{
			set.addEdge(edgeSets[n][permutation[n]]);
		}

		tasksets_.push_back(std::move(set));
	}

	return tasksets_;
}

const std::vector<DAG> &
VariableTaskSet::createDAGs(DataFiles *f)
{
	allDAGs_.clear();
	if (tasksets_.empty())
		createTasksets();

	unsigned k = 1;
	for (auto &set : tasksets_)
	{

		auto dags = set.createDAGs(f);

		std::cout << std::endl
				  << "Taskset " << k++ << "/" << tasksets_.size() << ": " << dags.size() << " created" << std::endl;

		allDAGs_.insert(allDAGs_.end(), dags.begin(), dags.end());
		std::cout << allDAGs_.size() << " total DAGs" << std::endl
				  << std::endl
				  << std::endl;
	}
	return allDAGs_;
}

float VariableTaskSet::getUtilization() const
{
	return baselineTaskset_.getUtilization();
}

PlainTaskSet
VariableTaskSet::getPlainTaskSet() const
{
	return baselineTaskset_.getPlainTaskSet();
}

const VariableMultiEdge &
VariableTaskSet::addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to)
{
	return addDataEdge(from, to, std::max(from->period, to->period) / std::min(from->period, to->period));
}

const VariableMultiEdge &
VariableTaskSet::addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to,
							 unsigned maxJitter)
{
	std::vector<unsigned> jitters;

	std::shared_ptr<MultiNode> fast;
	std::shared_ptr<MultiNode> slow;

	if (from->period > to->period)
	{
		fast = to;
		slow = from;
	}
	else
	{
		fast = from;
		slow = to;
	}
	unsigned jitterMin = std::max(int(std::ceil((2 * fast->wcet + slow->wcet) / fast->period) - 2), 0);
	std::cout << jitterMin << " " << maxJitter << std::endl;

	for (unsigned k = jitterMin; k <= std::min(std::max(from->period, to->period) / std::min(from->period, to->period), maxJitter); k++)
		jitters.push_back(k);
	return addDataEdge(from, to, jitters);
}

VariableTaskSet::~VariableTaskSet()
{

	// std::cout<<"Distruggo variable taskset---------------------------------------------------------"<<std::endl;
	// for (auto &d : allDAGs_)
	// 	d.freeMem();
	// allDAGs_.clear();
	// for (auto &t : tasksets_)
	// 	t.freeMem();
	// tasksets_.clear();

	// for (auto &e : edges_)
	// {
	// 	for (auto &n : e.from->nodes)
	// 		n.reset();
	// 	e.from->nodes.clear();
	// 	e.from.reset();

	// 	for (auto &n : e.to->nodes)
	// 		n.reset();
	// 	e.to->nodes.clear();
	// 	e.to.reset();

	// 	e.jitter.clear();
	// }
	// edges_.clear();
	// baselineTaskset_.freeMem();
	// std::cout<<"Distrutto---------------------------------------------------------"<<std::endl;
}