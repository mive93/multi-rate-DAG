/*
 * dag.h
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#ifndef DAG_H_
#define DAG_H_
#include <vector>

#include "Edge.h"
#include "Node.h"
class DAG
{
public:

	DAG();

	DAG(unsigned period);

	DAG(const DAG& other);

	void
	setPeriod(unsigned period);

	bool
	isCyclic();

	void
	transitiveReduction();

	void
	addNodes(const std::vector<std::shared_ptr<Node>>& nodes);

	void
	addEdges(const std::vector<std::shared_ptr<Edge>>& edges);

	unsigned
	getNumNodes() const;

	unsigned
	getNumEdges() const;

	std::vector<std::shared_ptr<Node>>
	getNodes() const;

	std::vector<std::shared_ptr<Edge>>
	getEdges() const;

	void
	printNodes() const;

	void
	printEdges() const;

	const std::shared_ptr<Node>&
	getEnd() const;

	const std::shared_ptr<Node>&
	getStart() const;

private:

	void
	createStartEnd();

	std::vector<std::shared_ptr<Node>> nodes_;
	std::vector<std::shared_ptr<Edge>> edges_;

	std::shared_ptr<Node> start_;
	std::shared_ptr<Node> end_;

	unsigned period_;

};

#endif /* DAG_H_ */
