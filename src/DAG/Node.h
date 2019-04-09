/*
 * Node.h
 *
 *  Created on: Apr 8, 2019
 *      Author: mirco
 */

#ifndef DAG_NODE_H_
#define DAG_NODE_H_
#include <memory>
#include <vector>

struct Edge;


struct Node
{
	unsigned offset;
	unsigned deadline;
	unsigned wcet;
	unsigned bcet;
	unsigned groupId;
//	MultiNode* parent;
//	unsigned instance;

	unsigned uniqueId;

	std::string name;

	std::vector<std::shared_ptr<Node>> prev;
	std::vector<std::shared_ptr<Node>> next;

	std::vector<std::shared_ptr<Edge>> edges;

	std::vector<std::shared_ptr<Node>>
	getAncestors();
};





#endif /* DAG_NODE_H_ */
