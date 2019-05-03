
/*
 * VariableMultiEdge.cpp
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */
#include <VariableTaskSet/VariableMultiEdge.h>




std::vector<MultiEdge>
VariableMultiEdge::translateToMultiEdges()
{
	std::vector<MultiEdge> edges;

	MultiEdge edge;
	edge.from = from;
	edge.to = to;
	edge.dependency = MultiEdge::Dependency::DATA;

	for (auto j : jitter)
	{
		edge.jitter = j;
		edges.push_back(edge);
	}

	return edges;
}
