/*
 * MultiRate.cpp
 *
 *  Created on: Apr 3, 2019
 *      Author: mirco
 */
#include "MultiRate/MultiRateTaskset.h"
#include "MultiRate/MultiNode.h"

#include <vector>
#include <iostream>

MultiRateTaskset::MultiRateTaskset() :
		dummyNodes_(std::make_shared<DummyNodes>()), hyperPeriod_(0)
{
}

std::shared_ptr<MultiNode>
MultiRateTaskset::addTask(unsigned period, unsigned wcet, unsigned deadline,
		const std::string& name)
{
	auto mult = std::make_shared<MultiNode>();
	mult->period = period;
	mult->wcet = wcet;
	mult->bcet = wcet;
	mult->deadline = deadline;
	mult->id = nodes_.size() + 1; //id 0 is for start

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

const MultiEdge&
MultiRateTaskset::addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to)
{
	MultiEdge edge;
	edge.from = from;
	edge.to = to;
	edge.dependency = MultiEdge::Dependency::PRECEDENCE;
	edge.jitter = 0; //Precedence does not have jitter
	edges_.push_back(edge);

	return edges_.back();
}

const MultiEdge&
MultiRateTaskset::addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to,
		unsigned jitter)
{
	MultiEdge edge;
	edge.from = from;
	edge.to = to;
	edge.dependency = MultiEdge::Dependency::DATA;
	edge.jitter = jitter;
	edges_.push_back(edge);

	return edges_.back();
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
		std::vector<Edge> edges;
		edges.push_back(Edge(start, nodes.front()));
		for (unsigned k = 0; k < nodes.size() - 1; k++)
		{
			edges.push_back(Edge(nodes[k], nodes[k + 1]));
		}
		edges.push_back(Edge(nodes.back(), end));
		baselineDAG_.addNodes(nodes);
		baselineDAG_.addEdges(edges);
	}

	dummyNodes_->addToDAG(baselineDAG_, hyperPeriod_);

	std::cout << "Baseline DAG created" << std::endl;
	std::cout << "Number of Nodes: " << baselineDAG_.getNumNodes() << std::endl;
	std::cout << "Number of Edges: " << baselineDAG_.getNumEdges() << std::endl;
	// Set Unique IDs
	auto nodes = baselineDAG_.getNodes();
	for (unsigned k = 0; k < nodes.size(); k++)
		nodes[k]->uniqueId = k;
	return baselineDAG_;
}

const std::vector<DAG>&
MultiRateTaskset::createDAGs()
{
	std::vector<std::vector<std::vector<Edge>>> edgeSets;

	std::vector<int> permutSets;
	for (auto& edge : edges_)
	{
		edgeSets.push_back(edge.translateToEdges());
		permutSets.push_back(edgeSets.back().size());
	}
	permutSets.push_back(1);

	std::vector<int> permutation(edgeSets.size(), 0);
	int numPermutations = 1;
	for (const auto& it : edgeSets)
		numPermutations *= it.size();

	for (int k = permutSets.size() - 2; k >= 0; k--)
	{
		permutSets[k] = permutSets[k + 1] * permutSets[k];
	}

	int cyclicDags = 0;
	int brokenDummyChain = 0;
	int wcetFailure = 0;
	int jitterFailure = 0;

	std::cout << numPermutations << " Permutations available" << std::endl;

	dags_.clear();
	for (int k = 0; k < numPermutations; k++)
	{
		std::cout << k << "/" << numPermutations;
		DAG dag(baselineDAG_);

		int tmp = k;
		for (unsigned i = 0; i < permutation.size(); i++)
		{
			permutation[i] = tmp / permutSets[i + 1];
			tmp = tmp % permutSets[i + 1];
		}

		for (unsigned n = 0; n < edgeSets.size(); n++)
		{
			dag.addEdges(edgeSets[n][permutation[n]]);
		}

		if (dag.isCyclic())
		{
			std::cout << " excluded because it is cyclic" << std::endl;
			cyclicDags++;
			continue;
		}

		dag.createMats();

		dag.transitiveReduction();

		//Check if Dummy chain was broken, making the DAG not schedulable
//		if (dummyNodes_->brokenDummyChain(dag))
//		{
//			brokenDummyChain++;
//			continue;
//		}

		//Check WCET sum in the chains
		if (!dag.checkLongestChain())
		{
			std::cout << " excluded because longest chain too long" << std::endl;
			wcetFailure++;
			continue;
		}

		if (!checkJitter(dag))
		{
			std::cout << " excluded because jitter incorrect" << std::endl;
			jitterFailure++;
			continue;
		}

//		for (const auto& other : dags_)
//		{
//			if ((other.toDAGMatrix() - dag.toDAGMatrix()).isZero())
//			{
//				sameDags++;
//				continue;
//			}
//		}

		std::cout << " is fine" << std::endl;
		dags_.push_back(std::move(dag));
	}

	std::cout << cyclicDags << " cyclic Dags were excluded" << std::endl;
	std::cout << brokenDummyChain << " Dags were excluded due to broken Dummy Chain" << std::endl;
	std::cout << wcetFailure << " Dags were removed because the chains are too long" << std::endl;
	std::cout << jitterFailure << " Dags were removed because the parallelism is incorrect"
			<< std::endl;
	std::cout << dags_.size() << " valid DAGs were created" << std::endl;

	return dags_;
}

const std::vector<MultiEdge>&
MultiRateTaskset::getEdges() const
{
	return edges_;
}

std::shared_ptr<DummyNodes>
MultiRateTaskset::getDummyNodes() const
{
	return dummyNodes_;
}

bool
MultiRateTaskset::checkJitter(const DAG& dag) const
{
	auto jitterMat = dag.getJitterMatrix();
	auto groupMat = dag.getGroupMatrix(nodes_.size());

	Eigen::MatrixXi parMat = groupMat.transpose() * jitterMat * groupMat;

	bool correct = true;
	for (const auto& edge : edges_)
	{
		int from = edge.from->id;
		int to = edge.to->id;

		int normFactor = hyperPeriod_ / std::max(edge.from->period, edge.to->period);
		float jitter = (float)parMat.coeff(from - 1, to - 1) / normFactor;
		if ((float) edge.jitter != jitter)
		{

//			std::cout << "Jitter from " << from << " to " << to << " should be " << edge.jitter
//					<< ", but is " << (float)parMat.coeff(from - 1, to - 1) / normFactor
//					<< " with normFac " << normFactor << std::endl;
			correct = false;
		}
	}
	return correct;
}
