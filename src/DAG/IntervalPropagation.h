/*
 * IntervalPropagation.h
 *
 *  Created on: May 11, 2019
 *      Author: mirco
 */

#ifndef DAG_INTERVALPROPAGATION_H_
#define DAG_INTERVALPROPAGATION_H_
#include <ostream>
#include <utility>



struct IntervalPropagation
{
	std::pair<float, float> startInterval;
	std::pair<float, float> interval;

	void
	shiftLeft(float val)
	{
		interval.first -= val;
		interval.second -= val;
	}

	void
	cutRight(float val)
	{
		interval.second -= val;
		if (interval.second < interval.first)
			interval.first = interval.second;

		startInterval.second -= val;
		if (startInterval.second < startInterval.first)
			startInterval.second = startInterval.first;
	}

	void
	cutLeft(float val)
	{
		interval.first += val;
		startInterval.first += val;
	}

	void
	addHyperPeriod(float period)
	{
		startInterval.first += period;
		startInterval.second += period;
		interval.first += period;
		interval.second += period;
	}

	bool
	canReact(float start, float end) const
	{
		return interval.second >= start;
	}

	void
	react(float start, float end)
	{
		if (interval.first < start)
			cutLeft(start - interval.first);

		if (interval.second > end)
			cutRight(interval.second - end);

	}

	void
	shiftWriteRead(float bcet, float rEnd)
	{
		shiftLeft(bcet);

		if (interval.second > rEnd)
			cutRight(interval.second - rEnd);
	}

};

std::ostream&
operator <<(std::ostream& o, const IntervalPropagation& p)
{
	o << "[" << p.startInterval.first << ", " << p.startInterval.second << "] : " << "[" << p.interval.first << ", " << p.interval.second << "]";
	return o;
}


#endif /* DAG_INTERVALPROPAGATION_H_ */
