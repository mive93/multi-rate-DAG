/*
 * Edge.h
 *
 *  Created on: Apr 8, 2019
 *      Author: mirco
 */

#ifndef DAG_EDGE_H_
#define DAG_EDGE_H_
#include <memory>

struct Node;

struct Edge
{
	Edge();

	Edge(std::shared_ptr<Node> from, std::shared_ptr<Node> to);

	void
	flipEdge();

	std::shared_ptr<Node> from;
	std::shared_ptr<Node> to;

};

#endif /* DAG_EDGE_H_ */
