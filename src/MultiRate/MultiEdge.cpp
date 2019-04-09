/*
 * MultiEdge.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mirco
 */
#include <MultiRate/MultiEdge.h>
#include <DAG/Edge.h>

#include <iostream>

std::vector<std::vector<std::shared_ptr<Edge>> >
MultiEdge::translateToEdges()
{
	auto fromNodes = from->nodes;
	auto toNodes = to->nodes;

	int numFrom = fromNodes.size();
	int numTo = toNodes.size();

	std::vector<std::vector<std::shared_ptr<Edge>>> edgeSets;

	unsigned maxPeriod = std::max(to->period, from->period);
	unsigned minPeriod = std::min(to->period, from->period);
	unsigned ratio = maxPeriod / minPeriod;

	if (dependency == Dependency::PRECEDENCE)
	{
		std::vector<std::shared_ptr<Edge>> edgeSet;
		int min = std::min(numTo, numFrom);
		for (int k = 0; k < min; k++)
		{
			std::shared_ptr<Edge> edge;
			if (numFrom > numTo)
			{
				int from = numFrom / min * (k + 1) - 1;
				int to = k;
				edge = std::make_shared<Edge>(fromNodes[from], toNodes[to]);
			}
			else
			{
				int from = k;
				int to = numTo / min * k;
				edge = std::make_shared<Edge>(fromNodes[from], toNodes[to]);
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

	for (int m = 0; m < numPermutations; m++)
	{

		std::vector<std::shared_ptr<Edge>> edgeSet;
		int min = std::min(numTo, numFrom);
		for (int k = 0; k < min; k++)
		{

			int from = -1 + m + numFrom / min * k;
			int to = from + jitter + 1;

			if (from >= numFrom / min * k)
			{
				edgeSet.push_back(std::make_shared<Edge>(fromNodes[from], toNodes[k]));
			}

			if (to < numFrom / min * (k + 1))
			{
				edgeSet.push_back(std::make_shared<Edge>(toNodes[k], fromNodes[to]));
			}

		}
		edgeSets.push_back(edgeSet);
	}

	return edgeSets;

}
