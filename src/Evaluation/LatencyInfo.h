/*
 * LatencyInfo.h
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */

#ifndef EVALUATION_LATENCYINFO_H_
#define EVALUATION_LATENCYINFO_H_
#include <limits>
#include <ostream>

struct LatencyInfo
{
	float minLatency; //Not implemented yet
	float maxLatency; // Also Data Age
	float reactionTime;

	std::pair<unsigned, unsigned> minLatencyPair;
	std::pair<unsigned, unsigned> maxLatencyPair;
	std::pair<unsigned, unsigned> reactionTimePair;

};

struct LatencyCost
{
	float maxLatency;
	float reactionTime;

	LatencyCost() :
			maxLatency(0.0), reactionTime(0.0)
	{
	}

	LatencyCost(float ageCost, float reactCost) :
			maxLatency(ageCost), reactionTime(reactCost)
	{
	}

	float
	getCost(const LatencyInfo& info) const
	{
		return maxLatency * info.maxLatency + reactionTime * info.reactionTime;
	}
};

struct LatencyConstraint
{
	float maxDataAge;
	float maxReactionTime;

	LatencyConstraint() :
			maxDataAge(std::numeric_limits<float>::max()), maxReactionTime(
					std::numeric_limits<float>::max())
	{
	}

	LatencyConstraint(float maxAge, float maxReact) :
			maxDataAge(maxAge), maxReactionTime(maxReact)
	{
	}

	bool
	isValid(const LatencyInfo& info) const
	{
		return info.maxLatency <= maxDataAge && info.reactionTime <= maxReactionTime;
	}
};

inline std::ostream&
operator <<(std::ostream& o, const LatencyInfo& info)
{
	o << "MinLatency: " << info.minLatency << " [" << info.minLatencyPair.first << "-"
			<< info.minLatencyPair.second << "]" << std::endl;
	o << "MaxLatency: " << info.maxLatency << " [" << info.maxLatencyPair.first << "-"
			<< info.maxLatencyPair.second << "]" << std::endl;
	o << "ReactionTime: " << info.reactionTime << " [" << info.reactionTimePair.first << "-"
			<< info.reactionTimePair.second << "]" << std::endl;
	return o;
}

#endif /* EVALUATION_LATENCYINFO_H_ */
