/*
 * dag.h
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#ifndef DAG_H_
#define DAG_H_
#include <Evaluation/LatencyInfo.h>
#include <eigen3/Eigen/Core>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "DAG/Edge.h"
#include "DAG/Node.h"

struct MultiEdge;
class MultiRateTaskset;

class DAG
{
public:

	struct NodeInfo
	{
		Eigen::VectorXf bc; //Best case execution time
		Eigen::VectorXf wc; //Worst case execution time

		Eigen::VectorXf est; //Earliest Starting Time
		Eigen::VectorXf lst; //Latest Starting Time
		Eigen::VectorXf eft; //Earliest Finishing Time
		Eigen::VectorXf lft; //Latest Finishing Time

		friend std::ostream &
		operator <<(std::ostream &out, const NodeInfo &c);
	};

	using DAGMatrix = Eigen::Matrix<int, -1, -1>;
	using DAGMatrixFloat = Eigen::Matrix<float, -1, -1>;

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

	DAGMatrix
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

	unsigned
	getPeriod() const
	{
		return period_;
	}

	void
	createMats();

	void
	createNodeInfo();

	const NodeInfo&
	getNodeInfo() const
	{
		return nodeInfo_;
	}

	const DAGMatrix&
	getDefinitelySerialized() const
	{
		return definitelySerialized_;
	}

	LatencyInfo
	getLatencyInfo(const std::vector<unsigned>& dataChain) const;

	LatencyInfo
	getMinLatency(const std::vector<unsigned>& dataChain, LatencyInfo& latency) const;

	MultiRateTaskset*
	getOriginatingTaskset() const
	{
		return originatingTaskset_;
	}

	void
	setOriginatingTaskset(MultiRateTaskset* originatingTaskset)
	{
		originatingTaskset_ = originatingTaskset;
	}

	const DAGMatrixFloat&
	getPartiallySerializedReact() const
	{
		return partiallySerializedReact_;
	}

	const DAGMatrixFloat&
	getPartiallySerialized() const
	{
		return partiallySerialized_;
	}

	const DAGMatrixFloat&
	getPartiallySerializedReactBInit() const
	{
		return partiallySerializedReactBInit_;
	}
private:

	void
	createStartEnd();

	void
	convertToBooleanMat(DAGMatrix& mat) const;

	void
	convertToBooleanVec(Eigen::VectorXi& vec) const;

	std::vector<unsigned>
	getIdsFromGroup(const DAGMatrix& groupMat, unsigned group) const;

	DAGMatrix dagMatrix_;
	DAGMatrix ancestors_;
	DAGMatrix definitelySerialized_;

	DAGMatrixFloat partiallySerialized_;
	DAGMatrixFloat partiallySerializedReact_;
	DAGMatrixFloat partiallySerializedReactBInit_;

	std::vector<std::shared_ptr<Node>> nodes_;
	std::vector<Edge> edges_;
	NodeInfo nodeInfo_;

	std::shared_ptr<Node> start_;
	std::shared_ptr<Node> end_;

	unsigned period_;

	MultiRateTaskset* originatingTaskset_;

};

#endif /* DAG_H_ */
