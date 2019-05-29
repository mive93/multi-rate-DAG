/*
 * ChainSim.cpp
 *
 *  Created on: May 19, 2019
 *      Author: mirco
 */
#include <Simulation/ChainSim.h>
#include <Simulation/JitterCount.h>

void
ChainSim::read(unsigned task, const TimePoint& time)
{
	for (unsigned id = 0; id < chain.size(); id++)
	{
		if (chain[id] == task)
		{
			if (id == 0)
			{
				//Add as new sample
				auto sample = std::make_pair(time, std::vector<uint8_t>(chain.size(), 0));
				sample.second.front() = 1;
				data.push_back(sample);

				continue;
			}

			for (auto& sample : data)
			{
				//Check if previous task wrote value
				if (sample.second[id - 1] == 2 && sample.second[id] == 0)
				{
					sample.second[id] = 1;
				}
			}
		}
	}

}

void
ChainSim::write(unsigned task, const TimePoint& time)
{
	for (unsigned id = 0; id < chain.size(); id++)
	{
		if (chain[id] == task)
		{
			if (id == chain.size() - 1)
			{
				bool ageSet = false;
				int ageIdx = 0;
				for (int k = data.size() - 1; k >= 0; k--)
				{
					//Check if value was read
					if (data[k].second[id] != 0)
					{
						if (!ageSet)
						{
							//We will definitely react to the last in the chain, even if it was reacted to already
							ages.push_back(time - data[k].first);
							ageSet = true;
							ageIdx = k;
						}
						if (data[k].second[id] == 1)
						{
							//First time reacting to it
							reactions.push_back(time - data[k].first);
						}

						data[k].second[id] = 2;
					}
				}
				if (ageIdx != 0)
				{
					data.erase(data.begin(), data.begin() + ageIdx);
				}
			}

			for (auto& sample : data)
			{
				//Check if value was read
				if (sample.second[id] == 1)
					sample.second[id] = 2;
			}
		}
	}

}

