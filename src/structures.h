/*
 * structures.h
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include <vector>
#include <memory>

enum class Engine
{
	CPU, GPU, FPGA
};

struct HeterogeneousInfo
{
	Engine engine;
	unsigned processorId;
};

struct Node
{
	unsigned offset;
	unsigned deadline;
	unsigned wcet;
//	MultiNode* parent;
//	unsigned instance;

	unsigned uniqueId;

	std::vector<std::shared_ptr<Node>> prev;
	std::vector<std::shared_ptr<Node>> next;

	std::vector<std::shared_ptr<Node>>
	getAncestors();
};

struct MultiNode
{
	unsigned id;

	unsigned period;
	unsigned deadline;
	unsigned wcet;

	HeterogeneousInfo* info = nullptr;

	void
	createNodes(unsigned hyperPeriod, std::shared_ptr<Node> globalS, std::shared_ptr<Node> globalE);

	std::vector<std::shared_ptr<Node>> nodes;
};

struct MultiEdge
{
	MultiNode* from;
	MultiNode* to;
	int jitter;
};

struct Edge
{
	Node* from;
	Node* to;
};

bool
isChain(const std::vector<std::shared_ptr<Node>>& nodes);

#endif /* STRUCTURES_H_ */
