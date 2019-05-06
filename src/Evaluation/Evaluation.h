/*
 * Evaluation.h
 *
 *  Created on: May 3, 2019
 *      Author: mirco
 */

#ifndef EVALUATION_EVALUATION_H_
#define EVALUATION_EVALUATION_H_
#include <DAG/DAG.h>
#include <Evaluation/LatencyInfo.h>
#include <Evaluation/SchedulingInfo.h>
#include <MultiRate/MultiNode.h>

class Evaluation
{
public:

	using Chain = std::vector<std::shared_ptr<MultiNode>>;

	void
	addLatency(const Chain& chain, const LatencyCost& cost, const LatencyConstraint& constraint);

	void
	addScheduling(const SchedulingCost& cost, const SchedulingConstraint& constraint);

	const DAG&
	evaluate(const std::vector<DAG>& dags);

	std::vector<unsigned>
	taskChainToNum(const Chain& chain);

private:

	void
	printChain(const Chain& chain);

	SchedulingInfo
	getSchedulingInfo(const DAG& dag, const SchedulingConstraint& constraint);



	std::vector<std::pair<Chain, std::pair<LatencyCost, LatencyConstraint>>> latencyEval_;

	std::pair<SchedulingCost, SchedulingConstraint> schedulingEval_;

};

#endif /* EVALUATION_EVALUATION_H_ */
