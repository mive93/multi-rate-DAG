/*
 * SchedulingInfo.h
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */

#ifndef EVALUATION_SCHEDULINGINFO_H_
#define EVALUATION_SCHEDULINGINFO_H_
#include <limits>

struct SchedulingInfo
{
	SchedulingInfo() = default;

	SchedulingInfo(unsigned m) :
			numCoresNeeded(m)
	{
	}

	unsigned numCoresNeeded = 1;
};

struct SchedulingCost
{
	float coreCost = 0;

	SchedulingCost() = default;

	SchedulingCost(float cores) :
			coreCost(cores)
	{
	}

	float
	getCost(const SchedulingInfo& info)
	{
		return coreCost * info.numCoresNeeded;
	}
};

struct SchedulingConstraint
{
	unsigned maxCores = std::numeric_limits<unsigned>::max();

	SchedulingConstraint() = default;

	SchedulingConstraint(unsigned cores) :
			maxCores(cores)
	{
	}

	bool
	isValid(const SchedulingInfo& info)
	{
		return info.numCoresNeeded <= maxCores;
	}
};

namespace dp
{
template<class Archive, typename>
inline void
serialize(Archive& ar, SchedulingInfo& info)
{
	ar & info.numCoresNeeded;
}

template<class Archive, typename>
inline void
serialize(Archive& ar, SchedulingCost& info)
{
	ar & info.coreCost;
}

template<class Archive, typename>
inline void
serialize(Archive& ar, SchedulingConstraint& info)
{
	ar & info.maxCores;
}
}

#endif /* EVALUATION_SCHEDULINGINFO_H_ */
