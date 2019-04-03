/*
 * MultiRate.h
 *
 *  Created on: Apr 3, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_H_
#define MULTIRATE_H_

#include <memory>
#include "structures.h"

class MultiRateTaskset
{
public:

	MultiRateTaskset();

	std::shared_ptr<MultiNode>
	addTask(unsigned period, unsigned wcet, unsigned deadline);

	std::shared_ptr<MultiNode>
	addTask(unsigned period, unsigned wcet);

	std::shared_ptr<MultiEdge>
	addPrecedenceEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to);

	std::shared_ptr<MultiEdge>
	addDataEdge(std::shared_ptr<MultiNode> from, std::shared_ptr<MultiNode> to, unsigned jitter);

private:

	std::vector<std::shared_ptr<MultiNode>> nodes_;
	std::vector<std::shared_ptr<MultiEdge>> edges_;
};

#endif /* MULTIRATE_H_ */
