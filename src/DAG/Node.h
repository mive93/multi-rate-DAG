/*
 * Node.h
 *
 *  Created on: Apr 8, 2019
 *      Author: mirco
 */

#ifndef DAG_NODE_H_
#define DAG_NODE_H_
#include <memory>
#include <vector>
#include <functional>

struct Node
{
	inline
	Node() :
			offset(0), deadline(0), wcet(0), bcet(0), groupId(0), uniqueId(0)
	{
	}

	inline
	Node(float o, float d, float wc, float bc, unsigned g) :
			offset(o), deadline(d), wcet(wc), bcet(bc), groupId(g), uniqueId(0)
	{
	}

	float offset;
	float deadline;
	float wcet;
	float bcet;
	unsigned groupId;

	unsigned uniqueId;

	std::function<float()> executionTimeGen = [this]{return this->wcet;};

	std::string name;
	std::string shortName;
};

#endif /* DAG_NODE_H_ */
