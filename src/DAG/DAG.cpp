/*
 * dag.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */
#include "DAG.h"

#include <iostream>
DAG::DAG() :
		period_(0)
{
	createStartEnd();
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
						for (auto childOfParent = (*parent)->next.begin();
								childOfParent != (*parent)->next.end(); childOfParent++)
							if (*childOfParent == node)
								(*parent)->next.erase(childOfParent);
						node->prev.erase(parent);
					}
				}
			}
		}
	}

}

DAG::DAG(unsigned period) :
		period_(period)
{
	createStartEnd();
}

void
DAG::setPeriod(unsigned period)
{
	period_ = period;

	end_->deadline = period;
	end_->offset = period;
}

void
DAG::addNodes(const std::vector<std::shared_ptr<Node> >& nodes)
{
	nodes_.insert(nodes_.end(), nodes.begin(), nodes.end());
}

unsigned
DAG::getNumNodes() const
{
	return nodes_.size();
}

unsigned
DAG::getNumEdges() const
{
	return edges_.size();
}

void
DAG::printNodes() const
{
	std::cout << "[WCET, BCET, Offset, Deadline]" << std::endl << std::endl;
	for (auto node : nodes_)
	{
		std::cout << node->name << std::endl;
		std::cout << "[" << node->wcet << ", ";
		std::cout << node->bcet << ", ";
		std::cout << node->offset << ", ";
		std::cout << node->deadline << "]" << std::endl << std::endl;
	}
}

void
DAG::printEdges() const
{
	for (auto edge : edges_)
		std::cout << edge->from->name << " -> " << edge->to->name << std::endl;
}


void 
DAG::DAGtotikz(std::string filename) const
{
	//opening the tex file
	std::ofstream tikz_file;
	tikz_file.open (filename);

	//beginning the tikz figure
	tikz_file << "\\begin{tikzpicture}[shorten >=1pt,node distance=3cm,auto,bend angle=45]\n";

	const int distance = 4; //distance between the nodes
	int x = 0, y = 0; //x and y of the nodes
	int cur_groupId = -1;
	int max_x = 0;
	
	//figuring out number of rows and columns of the matrix
	for (auto node : nodes_)
	{
		if (node->name != "start" && node->name != "end")
		{
			x += distance;
			if (node->groupId != cur_groupId)
			{
				max_x = x > max_x ? x : max_x;
				x = distance;
				y += distance;	
			}
			cur_groupId = node->groupId;
		}
	}

	//actually inserting the nodes
	y /= 2;
	x = 0;
	cur_groupId = -1;

	for (auto node : nodes_)
	{
		if (node->name == "start")
		{
			tikz_file << "\\node[state, fill,draw=none,green, text=black] ("<< node->name<<") at (0,0) [below left of=t11]{"<< node->name<<"};\n";
			x = distance;
		}
		else if (node->name == "end")
		{
			tikz_file << "\\node[state, fill,draw=none,red,text=black]("<< node->name<<") at ("<<max_x<<",0) {"<< node->name<<"};\n";
		}
		else
		{
			x += distance;
			if (node->groupId != cur_groupId)
			{
				x = distance;
				y -= distance;	
			}
			cur_groupId = node->groupId;
			tikz_file << "\\node[state] ("<< node->name<<") at ("<<x<<","<<y<<") {"<<node->name<<"};\n";
		}
	}

	//inserting edges
	tikz_file <<"\\path[->] \n";
	for (auto edge : edges_)
		tikz_file << "("<< edge->from->name <<") edge node {} ("<< edge->to->name <<")\n";
	tikz_file << ";\n";
	
	//ending tikz figure
	tikz_file << "\\end{tikzpicture}\n";
	
	//close the file
	tikz_file.close();
}

void
DAG::addEdges(const std::vector<std::shared_ptr<Edge> >& edges)
{
	edges_.insert(edges_.end(), edges.begin(), edges.end());
}

std::vector<std::shared_ptr<Edge> >
DAG::getEdges() const
{
	return edges_;
}

void
DAG::createStartEnd()
{
	start_ = std::make_shared<Node>();
	start_->bcet = 0;
	start_->wcet = 0;
	start_->deadline = 0;
	start_->offset = 0;
	start_->uniqueId = 0;
	start_->name = "start";

	end_ = std::make_shared<Node>();
	end_->bcet = 0;
	end_->wcet = 0;
	end_->deadline = period_;
	end_->offset = period_;
	end_->uniqueId = 10000;
	end_->name = "end";

	nodes_.push_back(start_);
	nodes_.push_back(end_);
}

std::vector<std::shared_ptr<Node> >
DAG::getNodes() const
{
	return nodes_;
}

const std::shared_ptr<Node>&
DAG::getEnd() const
{
	return end_;
}

const std::shared_ptr<Node>&
DAG::getStart() const
{
	return start_;
}

DAG::DAG(const DAG& other) :
		nodes_(other.getNodes()), edges_(other.getEdges()), start_(other.getStart()), end_(
				other.getEnd()), period_(other.period_)
{
}
