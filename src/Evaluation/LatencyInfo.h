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

namespace dp
{
template<class Archive, typename>
inline void
serialize(Archive& ar, LatencyInfo& info)
{
	ar & info.maxLatency;
	ar & info.maxLatencyPair;
	ar & info.minLatency;
	ar & info.minLatencyPair;
	ar & info.reactionTime;
	ar & info.reactionTimePair;
}

template<class Archive, typename>
inline void
serialize(Archive& ar, LatencyCost& info)
{
	ar & info.maxLatency;
	ar & info.reactionTime;
}

template<class Archive, typename>
inline void
serialize(Archive& ar, LatencyConstraint& info)
{
	ar & info.maxDataAge;
	ar & info.maxReactionTime;
}
}

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

inline std::ostream&
operator <<(std::ostream& o, const LatencyCost& cost)
{
	o << "Cost max Latency: " << cost.maxLatency << std::endl;
	o << "Cost ReactionTime: " << cost.reactionTime << std::endl;
	return o;
}

inline std::ostream&
operator <<(std::ostream& o, const LatencyConstraint& constr)
{
	o << "Max Data Age: " << constr.maxDataAge << std::endl;
	o << "Max React Time: " << constr.maxReactionTime << std::endl;
	return o;
}

#endif /* EVALUATION_LATENCYINFO_H_ */
