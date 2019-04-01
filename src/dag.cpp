/*
 * dag.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */
#include "dag.h"

std::vector<DAG>
MultirateTaskset::createDAGs()
{
	return std::vector<DAG>();
}

bool
DAG::isCyclic()
{
	return false;
}

void
DAG::transitiveReduction()
{
	for (auto node : nodes_)
	{
		for (auto parent = node->prev.begin(); parent != node->prev.end(); parent++)
		{
			for (auto other : node->prev)
			{
				if (other == *parent)
					continue;

				for (auto it : other->getAncestors())
				{
					if (*parent == it)
					{
						for (auto childOfParent = (*parent)->next.begin(); childOfParent != (*parent)->next.end(); childOfParent++)
							if (*childOfParent == node)
								(*parent)->next.erase(childOfParent);
						node->prev.erase(parent);
					}
				}
			}
		}
	}

}
