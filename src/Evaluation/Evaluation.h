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
#include "Benchmark/DataFiles.h"

class Evaluation;
namespace dp
{
template<class Archive, typename >
void
serialize(Archive& ar, Evaluation& eval);
}

class TaskSet;
class ITimeProvider;

class Evaluation 
{
public:


	void
	addLatency(const std::vector<std::shared_ptr<MultiNode>>& chain, const LatencyCost& cost, const LatencyConstraint& constraint);

	void
	addChain(const std::vector<unsigned>& chain, const LatencyCost& cost = LatencyCost(), const LatencyConstraint& constraint = LatencyConstraint());

	void
	addScheduling(const SchedulingCost& cost, const SchedulingConstraint& constraint);

	const DAG&
	evaluate(const std::vector<DAG>& dags, DataFiles *f = nullptr);

	float
	evaluate(const DAG& dag);

	std::vector<unsigned>
	taskChainToNum(const std::vector<std::shared_ptr<MultiNode>>& chain);


private:

	void
	printChain(const std::vector<unsigned>& chain) const;

	SchedulingInfo
	getSchedulingInfo(const DAG& dag, const SchedulingConstraint& constraint);


	std::vector<std::pair<std::vector<unsigned>, std::pair<LatencyCost, LatencyConstraint>>> latencyEval_;
	std::pair<SchedulingCost, SchedulingConstraint> schedulingEval_;
	std::vector<std::vector<LatencyInfo>> latencies_;

};

#endif /* EVALUATION_EVALUATION_H_ */