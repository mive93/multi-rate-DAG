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

struct Edge;


struct Node
{
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
