/*
 * MultiEdge.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mirco
 */
#include <MultiRate/MultiEdge.h>
#include <DAG/Edge.h>

#include <iostream>

std::vector<std::vector<Edge> >
MultiEdge::translateToEdges()
{
	auto fromNodes = from->nodes;
	auto toNodes = to->nodes;

	int numFrom = fromNodes.size();
	int numTo = toNodes.size();

	std::vector<std::vector<Edge>> edgeSets;

	unsigned maxPeriod = std::max(to->period, from->period);
	unsigned minPeriod = std::min(to->period, from->period);

	if (dependency == Dependency::PRECEDENCE)
	{
		std::vector<Edge> edgeSet;
		int min = std::min(numTo, numFrom);
		for (int k = 0; k < min; k++)
		{
			Edge edge;
			if (numFrom > numTo)
			{
				int from = numFrom / min * (k + 1) - 1;
				int to = k;
				edge = Edge(fromNodes[from], toNodes[to]);
			}
			else
			{
				int from = k;
				int to = numTo / min * k;
				edge = Edge(fromNodes[from], toNodes[to]);
			}
			edgeSet.push_back(edge);
		}
		edgeSets.push_back(edgeSet);
		return edgeSets;
	}

	//Data dependency
	unsigned numPermutations = maxPeriod / minPeriod - jitter + 1;

	if (numFrom < numTo)
	{
		//Flip direction for convenience
		//Convert into high-rate to low-rate
		fromNodes = to->nodes;
		toNodes = from->nodes;
		numFrom = fromNodes.size();
		numTo = toNodes.size();
	}

	for (unsigned m = 0; m < numPermutations; m++)
	{

		std::vector<Edge> edgeSet;
		int min = std::min(numTo, numFrom);
		for (int k = 0; k < min; k++)
		{

			int from = -1 + m + numFrom / min * k;
			int to = from + jitter + 1;

			if (from >= numFrom / min * k)
			{
				edgeSet.push_back(Edge(fromNodes[from], toNodes[k]));
			}

			if (to < numFrom / min * (k + 1))
			{
				edgeSet.push_back(Edge(toNodes[k], fromNodes[to]));
			}

		}
		edgeSets.push_back(edgeSet);
	}

	return edgeSets;

}
