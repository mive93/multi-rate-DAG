/*
 * VariableMultiNode.h
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */

#ifndef VARIABLETASKSET_VARIABLEMULTINODE_H_
#define VARIABLETASKSET_VARIABLEMULTINODE_H_
#include <MultiRate/MultiNode.h>
#include <memory>
#include <string>


struct VariableMultiNode
{
//	unsigned offset;
	float deadline;
	std::vector<unsigned> periods;
	float wcet;
	float bcet;

	unsigned id;

	std::string name;

	std::vector<std::shared_ptr<MultiNode>> multiNodes;

	std::vector<std::shared_ptr<MultiNode>>&
	createMultiNodes();

};


#endif /* VARIABLETASKSET_VARIABLEMULTINODE_H_ */
