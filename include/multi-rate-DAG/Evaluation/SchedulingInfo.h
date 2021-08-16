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

#endif /* EVALUATION_SCHEDULINGINFO_H_ */
