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




class NodeSet
{
public:

private:
	Node start_;
	Node end_;

	std::vector<Node> nodes_;
};

class EdgeSet
{
public:

	EdgeSet(NodeSet* nodeSet);

	EdgeSet(const EdgeSet& other);

	std::vector<Edge>
	getEdges() const;

	NodeSet*
	getNodeSet() const;

private:

	std::vector<Edge> edges_;

	NodeSet* nodeSet_;

};

bool
isChain(const std::vector<std::shared_ptr<Node>>& nodes);

#endif /* STRUCTURES_H_ */
