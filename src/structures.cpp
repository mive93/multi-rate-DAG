/*
 * structures.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */
#include "structures.h"






bool
isChain(const std::vector<std::shared_ptr<Node>>& nodes)
{
	for (int k = 0; k < nodes.size() - 1; k++)
	{
		bool temp = false;
		for (auto next : nodes[k]->next)
		{
			if (next == nodes[k+1])
			{
				temp = true;
				break;
			}
		}
		if (!temp)
			return false;
	}

	return true;
}
