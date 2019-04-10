/*
 * Edge.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: mirco
 */
#include "DAG/Edge.h"
#include "DAG/Node.h"

Edge::Edge()
{
}

Edge::Edge(std::shared_ptr<Node> f, std::shared_ptr<Node> t):
		from(f), to(t)
{
}

void
Edge::flipEdge()
{
	auto temp = from;
	from = to;
	to = temp;
}
