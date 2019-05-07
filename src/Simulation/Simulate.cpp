/*
 * Simulate.cpp
 *
 *  Created on: May 6, 2019
 *      Author: mirco
 */

#include <uavAP/Core/Scheduler/MicroSimulator.h>

int cool = 0;

void
test(MicroSimulator* sim)
{
	cool++;
	sim->schedule(std::bind(test, sim), Milliseconds(300), Milliseconds(150));
}

int
main()
{

	MicroSimulator sim;

	sim.schedule(std::bind(test, &sim), Milliseconds(50), Milliseconds(100));

	sim.simulate(Seconds(5));

	std::cout << cool << std::endl;


}
