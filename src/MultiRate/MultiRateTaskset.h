/*
 * MultiRate.h
 *
 *  Created on: Apr 3, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_H_
#define MULTIRATE_H_

#include <memory>

#include "DAG/DAG.h"
#include "MultiRate/DummyNodes.h"
#include "MultiRate/MultiEdge.h"

struct MultiNode;

class MultiRateTaskset
{
public:

	MultiRateTaskset();

	std::shared_ptr<MultiNode>
	addTask(unsigned period, unsigned wcet, unsigned deadline, const std::string& name = std::string());

	std::shared_ptr<MultiNode>
	addTask(unsigned period, unsigned wcet, const std::string& name = std::string());

	const MultiEdge&
	addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to);

	const MultiEdge&
	addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to, unsigned jitter);

	const DAG&
	createBaselineDAG();

	const std::vector<DAG>&
	createDAGs();

	std::shared_ptr<DummyNodes>
	getDummyNodes() const;

	const std::vector<MultiEdge>&
	getEdges() const;

	bool
	checkJitter(const DAG& dag) const;

private:

	DAG baselineDAG_;
	std::vector<DAG> dags_;

	std::vector<std::shared_ptr<MultiNode>> nodes_;
	std::vector<MultiEdge> edges_;

	std::shared_ptr<DummyNodes> dummyNodes_;

	unsigned hyperPeriod_;
};

#endif /* MULTIRATE_H_ */
