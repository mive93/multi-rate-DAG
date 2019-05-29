/*
 * JitterCount.h
 *
 *  Created on: May 29, 2019
 *      Author: mirco
 */

#ifndef SIMULATION_JITTERCOUNT_H_
#define SIMULATION_JITTERCOUNT_H_
#include <map>
#include <vector>

struct JitterCounter
{
	std::map<unsigned, std::map<unsigned, std::vector<uint8_t>>> counter;

	void
	write(unsigned task);

	void
	read(unsigned task);

	const std::vector<uint8_t>&
	getJitterCount(unsigned from, unsigned to) const;

	void
	addJitterCount(unsigned from, unsigned to);
};



#endif /* SIMULATION_JITTERCOUNT_H_ */
