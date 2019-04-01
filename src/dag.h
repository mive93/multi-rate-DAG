/*
 * dag.h
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */

#ifndef DAG_H_
#define DAG_H_
#include "structures.h"
#include <vector>

class DAG
{
public:

	bool
	isCyclic();

	void
	transitiveReduction();

private:

	std::vector<std::shared_ptr<Node>> nodes_;
	std::vector<Edge> edges_;

	unsigned period_;


};

class MultirateTaskset
{
public:

	std::vector<DAG>
	createDAGs();

private:

	std::vector<MultiNode> nodes_;
	std::vector<MultiEdge> edges_;

};

#endif /* DAG_H_ */
