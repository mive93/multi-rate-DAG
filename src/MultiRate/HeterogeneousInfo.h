/*
 * HeterogeneousInfo.h
 *
 *  Created on: Apr 8, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_HETEROGENEOUSINFO_H_
#define MULTIRATE_HETEROGENEOUSINFO_H_


enum class Engine
{
	CPU, GPU, FPGA
};

struct HeterogeneousInfo
{
	Engine engine;
	unsigned processorId;
};



#endif /* MULTIRATE_HETEROGENEOUSINFO_H_ */
