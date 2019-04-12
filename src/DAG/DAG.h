/*
 * dag.h
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#ifndef DAG_H_
#define DAG_H_
#include <eigen3/Eigen/Core>
#include <vector>
#include <string>
#include <fstream>

#include "Edge.h"
#include "Node.h"

struct MultiEdge;

class DAG
{
public:

	struct Chain
	{
		std::vector<int> wcetsStack;
		std::vector<int> uniquesStack;

		int wcet = 0;
		std::vector<int> uniqueCount;
	};

	using DAGMatrix = Eigen::MatrixXi;

	DAG();

	DAG(unsigned period);

	DAG(const DAG& other);

	void
	setPeriod(unsigned period);

	bool
	isCyclic() const;

	void
	transitiveReduction();

	void
	addNodes(const std::vector<std::shared_ptr<Node>>& nodes);

	void
	addEdges(const std::vector<Edge>& edges);

	unsigned
	getNumNodes() const;

	unsigned
	getNumEdges() const;

	const std::vector<std::shared_ptr<Node>>&
	getNodes() const;

	const std::vector<Edge>&
	getEdges() const;

	void
	printNodes() const;

	void
	printEdges() const;

	void 
	toTikz(std::string filename) const;

	const std::shared_ptr<Node>&
	getEnd() const;

	const std::shared_ptr<Node>&
	getStart() const;

	DAGMatrix
	toDAGMatrix() const;

	bool
	hasEdge(const Edge& e) const;

	bool
	checkJitter(const std::vector<MultiEdge>& jitterInfo) const;

private:

	void
	createStartEnd();

	void
	chainRecursion(const std::vector<MultiEdge>& jitterInfo);

	std::vector<std::shared_ptr<Node>> nodes_;
	std::vector<Edge> edges_;

	std::shared_ptr<Node> start_;
	std::shared_ptr<Node> end_;

	unsigned period_;

};

#endif /* DAG_H_ */
