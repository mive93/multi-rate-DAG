/*
 * MultiEdge.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mirco
 */
#include <MultiRate/MultiEdge.h>
#include <DAG/Edge.h>

std::vector<std::vector<std::shared_ptr<Edge>> >
MultiEdge::translateToEdges()
{
	auto fromNodes = from->nodes;
	auto toNodes = to->nodes;

	std::vector<Edge> edges;

}
