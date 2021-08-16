/*
 * VariableMultiEdge.h
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */

#ifndef VARIABLETASKSET_VARIABLEMULTIEDGE_H_
#define VARIABLETASKSET_VARIABLEMULTIEDGE_H_
#include <MultiRate/MultiEdge.h>
#include <MultiRate/MultiNode.h>


struct VariableMultiEdge
{

	std::shared_ptr<MultiNode> from;
	std::shared_ptr<MultiNode> to;
	std::vector<unsigned> jitter;

	std::vector<MultiEdge>
	translateToMultiEdges();

};


#endif /* VARIABLETASKSET_VARIABLEMULTIEDGE_H_ */
