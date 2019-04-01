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
		for (auto parent = node.pred.begin(); parent != node.pred.end(); parent++)
		{
			for (auto other : node.pred)
			{
				if (other == *parent)
					continue;

				for (auto it : other->getAncestors())
				{
					if (*parent == it)
					{
						for (auto childOfParent = (*parent)->succ.begin(); childOfParent != (*parent)->succ.end(); childOfParent++)
							if (*childOfParent == node)
								(*parent)->succ.erase(childOfParent);
						node.pred.erase(parent);
					}
				}
			}
		}
	}

}
