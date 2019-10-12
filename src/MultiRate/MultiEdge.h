/*
 * MultiEdge.h
 *
 *  Created on: Apr 8, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_MULTIEDGE_H_
#define MULTIRATE_MULTIEDGE_H_
#include "MultiNode.h"

struct Edge;

struct MultiEdge
{
	enum class Dependency
	{
		PRECEDENCE, DATA
	};

	std::vector<std::vector<Edge> >
	translateToEdges();

	std::vector<std::vector<Edge> >
	translateToEdgesSaidi();

	std::shared_ptr<MultiNode> from;
	std::shared_ptr<MultiNode> to;
	Dependency dependency;
	unsigned jitter;
};

#endif /* MULTIRATE_MULTIEDGE_H_ */
