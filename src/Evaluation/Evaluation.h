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
#include <Simulation/ChainSim.h>
#include <Simulation/JitterCount.h>
#include <cpsCore/Aggregation/IAggregatableObject.h>
#include <cpsCore/Aggregation/ObjectHandleContainer.hpp>
#include <cpsCore/Synchronization/IRunnableObject.h>
#include <cpsCore/Utilities/TimeProvider/ITimeProvider.h>

class Evaluation;
namespace dp
{
template<class Archive, typename >
void
serialize(Archive& ar, Evaluation& eval);
}

class TaskSet;
class ITimeProvider;

class Evaluation : public IAggregatableObject, public IRunnableObject
{
public:


	void
	addLatency(const std::vector<std::shared_ptr<MultiNode>>& chain, const LatencyCost& cost, const LatencyConstraint& constraint);

	void
	addChain(const std::vector<unsigned>& chain, const LatencyCost& cost = LatencyCost(), const LatencyConstraint& constraint = LatencyConstraint());

	void
	addScheduling(const SchedulingCost& cost, const SchedulingConstraint& constraint);

	const DAG&
	evaluate(const std::vector<DAG>& dags);

	float
	evaluate(const DAG& dag);

	std::vector<unsigned>
	taskChainToNum(const std::vector<std::shared_ptr<MultiNode>>& chain);

	void
	printInfo() const;

	void
	notifyAggregationOnUpdate(const Aggregator& agg) override;

	bool
	run(RunStage stage) override;

	void
	exportLatency(const std::string& fileOffset);

	void
	exportReactionTimes(const std::string& filename);

	void
	exportDataAges(const std::string& filename);


	const std::vector<uint8_t>&
	getJitterCount(unsigned from, unsigned to) const;

	void
	addJitterCount(unsigned from, unsigned to);

	void
	exportJitterCount(const std::string& fileOffset);

private:

	void
	printChain(const std::vector<unsigned>& chain) const;

	SchedulingInfo
	getSchedulingInfo(const DAG& dag, const SchedulingConstraint& constraint);

	void
	initChainSims();

	void
	readTask(unsigned task);

	void
	writeTask(unsigned task);

	std::vector<std::pair<std::vector<unsigned>, std::pair<LatencyCost, LatencyConstraint>>> latencyEval_;

	std::pair<SchedulingCost, SchedulingConstraint> schedulingEval_;

	std::vector<ChainSim> chainSims_;
	JitterCounter jitterCounter_;

	std::vector<std::vector<LatencyInfo>> latencies_;

	ObjectHandleContainer<TaskSet> taskSet_;
	ObjectHandleContainer<ITimeProvider> timeProvider_;

	template<class Archive, typename >
	inline friend void
	dp::serialize(Archive& ar, Evaluation& eval)
	{
		ar & eval.latencyEval_;
		ar & eval.schedulingEval_;
	}

};

#endif /* EVALUATION_EVALUATION_H_ */
