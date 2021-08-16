/*
 * VariableTaskSet.h
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */

#ifndef VARIABLETASKSET_VARIABLETASKSET_H_
#define VARIABLETASKSET_VARIABLETASKSET_H_
#include <MultiRate/MultiRateTaskset.h>
#include <VariableTaskSet/VariableMultiEdge.h>
#include <vector>

struct VariableMultiNode;

class VariableTaskSet
{
public:

	VariableTaskSet() = default;

	std::shared_ptr<MultiNode>
	addTask(unsigned period, float wcet, float deadline, const std::string& name = std::string());

	std::shared_ptr<MultiNode>
	addTask(unsigned period, float wcet, const std::string& name = std::string());

	const MultiEdge&
	addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to);

	const VariableMultiEdge&
	addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to, std::vector<unsigned> jitters);

	MultiRateTaskset&
	createBaselineTaskset();

	const std::vector<MultiRateTaskset>&
	createTasksets();

	const std::vector<DAG>&
	createDAGs();

	float
	getUtilization() const;


private:

	std::vector<std::shared_ptr<MultiNode>> nodes_;
	std::vector<VariableMultiEdge> edges_;

	std::vector<MultiRateTaskset> tasksets_;

	MultiRateTaskset baselineTaskset_;

	std::vector<DAG> allDAGs_;



};




#endif /* VARIABLETASKSET_VARIABLETASKSET_H_ */
