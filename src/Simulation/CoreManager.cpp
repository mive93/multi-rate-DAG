/*
 * CoreManager.cpp
 *
 *  Created on: May 13, 2019
 *      Author: mirco
 */
#include <Simulation/CoreManager.h>
#include <uavAP/Core/Logging/APLogger.h>




void
CoreManager::setNumCores(unsigned numCores)
{
	totalCores_(numCores);
}

void
CoreManager::notifyAggregationOnUpdate(const Aggregator& agg)
{
	dagScheduler_.setFromAggregationIfNotSet(agg);
}

bool
CoreManager::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!dagScheduler_.isSet())
		{
			APLOG_ERROR << "DAGScheduler missing.";
			return true;
		}
		break;
	}
	case RunStage::NORMAL:
	{
		availableCores_ = totalCores_;
		break;
	}
	default:
		break;
	}
	return false;
}
