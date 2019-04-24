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
#include <map>
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
		std::vector<int> nodesStack;

		unsigned wcet = 0;

	};

	struct NodeInfo
	{
		unsigned earliestArrival;
		unsigned latestArrival;

		std::shared_ptr<Node> node;
	};

	using DAGMatrix = Eigen::Matrix<int, -1, -1>;

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

	void
	fromDAGMatrix(const DAGMatrix& mat);

	Eigen::Matrix<int, -1, -1>
	getJitterMatrix() const;

	bool
	hasEdge(const Edge& e) const;

	bool
	checkLongestChain() const;

	Eigen::MatrixXi
	getGroupMatrix(int N) const;

	const DAGMatrix&
	getAncestors() const
	{
		return ancestors_;
	}

	const DAGMatrix&
	getDAGMatrix() const
	{
		return dagMatrix_;
	}

//	const DAGMatrix&
//	getDescendents() const
//	{
////		return descendents_;
//	}

	void
	createMats();

	void
	createNodeInfo();

	const std::vector<NodeInfo>&
	getNodeInfo() const
	{
		return nodeInfo_;
	}

private:

	void
	createStartEnd();

	void
	chainRecursionWCET(Chain& chain, const std::vector<std::vector<int>>& children) const;

	void
	convertToBooleanMat(DAGMatrix& mat) const;

	void
	convertToBooleanVec(Eigen::VectorXi& vec) const;

	DAGMatrix dagMatrix_;
	DAGMatrix ancestors_;

	std::vector<std::shared_ptr<Node>> nodes_;
	std::vector<Edge> edges_;
	std::vector<NodeInfo> nodeInfo_;

	std::shared_ptr<Node> start_;
	std::shared_ptr<Node> end_;

	unsigned period_;

};

#endif /* DAG_H_ */
