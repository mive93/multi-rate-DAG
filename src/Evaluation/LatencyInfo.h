/*
 * LatencyInfo.h
 *
 *  Created on: May 2, 2019
 *      Author: mirco
 */

#ifndef EVALUATION_LATENCYINFO_H_
#define EVALUATION_LATENCYINFO_H_
#include <ostream>

struct LatencyInfo
{
	unsigned minLatency; //Not implemented yet
	unsigned maxLatency; // Also Data Age
	unsigned reactionTime;

	std::pair<unsigned, unsigned> minLatencyPair;
	std::pair<unsigned, unsigned> maxLatencyPair;
	std::pair<unsigned, unsigned> reactionTimePair;

};

inline std::ostream&
operator <<(std::ostream& o, const LatencyInfo& info)
{
	o << "MinLatency: " << info.minLatency << " [" << info.minLatencyPair.first << "-" << info.minLatencyPair.second << "]" << std::endl;
	o << "MaxLatency: " << info.maxLatency << " [" << info.maxLatencyPair.first << "-" << info.maxLatencyPair.second << "]" << std::endl;
	o << "ReactionTime: " << info.reactionTime << " [" << info.reactionTimePair.first << "-" << info.reactionTimePair.second << "]" << std::endl;
	return o;
}

#endif /* EVALUATION_LATENCYINFO_H_ */
