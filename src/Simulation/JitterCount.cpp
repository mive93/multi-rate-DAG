/*
 * JitterCount.cpp
 *
 *  Created on: May 29, 2019
 *      Author: mirco
 */
#include <Simulation/JitterCount.h>
#include <utility>

const std::vector<uint8_t>&
JitterCounter::getJitterCount(unsigned from, unsigned to) const
{
	auto it = counter.find(from);
	if (it == counter.end())
		return std::vector<uint8_t>();

	auto itTo = it->second.find(to);
	if (itTo == it->second.end())
		return std::vector<uint8_t>();

	return itTo->second;
}

void
JitterCounter::write(unsigned task)
{
	auto collect = counter.find(task);
	if (collect == counter.end())
	{
		return;
	}

	for (auto& it : collect->second)
	{
		it.second.back()++;
	}
}

void
JitterCounter::read(unsigned task)
{
	for (auto& it : counter)
	{
		auto collect = it.second.find(task);
		if (collect == it.second.end())
			continue;
		collect->second.push_back(0);
	}
}

void
JitterCounter::addJitterCount(unsigned from, unsigned to)
{
	auto it = counter.find(from);
	if (it == counter.end())
	{
		std::map<unsigned, std::vector<uint8_t>> m;
		std::vector<uint8_t> c({0});
		m.insert(std::make_pair(to, c));
		counter.insert(std::make_pair(from, m));
		return;
	}

	auto itTo = it->second.find(to);
	if (itTo == it->second.end())
	{
		std::vector<uint8_t> c({0});
		it->second.insert(std::make_pair(to, c));
		return;
	}
}
