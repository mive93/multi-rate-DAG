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

struct Node
{
	inline
	Node() :
			offset(0), deadline(0), wcet(0), bcet(0), groupId(0), uniqueId(0)
	{
	}
	;

	inline
	Node(unsigned o, unsigned d, unsigned wc, unsigned bc, unsigned g) :
			offset(o), deadline(d), wcet(wc), bcet(bc), groupId(g), uniqueId(0)
	{
	}
	;

	unsigned offset;
	unsigned deadline;
	unsigned wcet;
	unsigned bcet;
	unsigned groupId;

	unsigned uniqueId;

	std::string name;
	std::string shortName;
};

#endif /* DAG_NODE_H_ */
