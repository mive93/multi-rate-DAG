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
DAG::isCyclic() const
{
	unsigned n = nodes_.size();
	DAGMatrix mat = toDAGMatrix();

	Eigen::VectorXi step = Eigen::VectorXi::Zero(n);
	step[0] = 1;

	for (int k = 0; k < n; ++k)
	{
		step = mat * step;
		if (step.isZero())
			return false;
	}

	return true;
}

void
DAG::transitiveReduction()
{
	//TODO Problem: Edges that are inserted twice are recognized as transitive and removed!! Fix somehow
	std::vector<Edge> newEdges;

	int oldNumEdges = edges_.size();
	for (auto& edge : edges_)
	{
		edge.flipEdge();
		bool transitiveEdge = isCyclic();
		edge.flipEdge();
		if (!transitiveEdge)
			newEdges.push_back(edge);
	}
	edges_ = newEdges;
	int newNumEdges = edges_.size();

//	std::cout << "Removed " << oldNumEdges - newNumEdges << "/" << oldNumEdges << " Edges"
//			<< std::endl;

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
	for (const auto& edge : edges_)
		std::cout << edge.from->name << " -> " << edge.to->name << std::endl;
}

void
DAG::toTikz(std::string filename) const
{
	//opening the tex file
	std::ofstream tikz_file;
	tikz_file.open(filename);

	//beginning the tikz figure
	tikz_file << "\\begin{tikzpicture}[shorten >=1pt,node distance=3cm,auto,bend angle=45]\n";

	const int distance = 2; //distance between the nodes
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
				x = distance;
				y += distance;
			}
			cur_groupId = node->groupId;
			max_x = x > max_x ? x : max_x;
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
			tikz_file << "\\node[state, fill,draw=none,green, text=black] (" <<
				node->shortName << ") at (0,0) [below left of=t11]{" << 
				node->shortName << "};\n";
			x = distance;
		}
		else if (node->name == "end")
		{
			tikz_file << "\\node[state, fill,draw=none,red,text=black](" << 
				node->shortName << ") at (" << max_x + distance << ",0) {" << 
				node->shortName << "};\n";
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
			
			if(node->groupId == 666)
				tikz_file << "\\node[state, fill,draw=none,blue,text=white] (" 
					<< node->shortName << ") at (" << x << "," << y << ") {"
					<< node->shortName << "};\n";
			else
				tikz_file << "\\node[state] (" << node->shortName << 
					") at (" << x << "," << y << ") {"<< node->shortName << "};\n";
		}
	}

	//inserting edges
	tikz_file << "\\path[->] \n";
	for (const auto& edge : edges_)
		tikz_file << "(" << edge.from->shortName << ") edge node {} (" << edge.to->shortName << ")\n";
	tikz_file << ";\n";

	//ending tikz figure
	tikz_file << "\\end{tikzpicture}\n";

	//close the file
	tikz_file.close();
}

void
DAG::addEdges(const std::vector<Edge>& edges)
{
	edges_.insert(edges_.end(), edges.begin(), edges.end());
}

const std::vector<Edge>&
DAG::getEdges() const
{
	return edges_;
}

DAG::DAGMatrix
DAG::toDAGMatrix() const
{
	unsigned n = nodes_.size();
	DAGMatrix mat = DAGMatrix::Zero(n, n);

	for (const auto& edge : edges_)
	{
		mat.col(edge.from->uniqueId)[edge.to->uniqueId] = 1;
	}
	return mat;
}

bool
DAG::hasEdge(const Edge& e) const
{
	for (const auto& edge : edges_)
	{
		if (e.from == edge.from && e.to == edge.to)
			return true;
	}
	return false;
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
	start_->shortName = "S";

	end_ = std::make_shared<Node>();
	end_->bcet = 0;
	end_->wcet = 0;
	end_->deadline = period_;
	end_->offset = period_;
	end_->uniqueId = 10000;
	end_->name = "end";
	end_->shortName = "E";

	nodes_.push_back(start_);
	nodes_.push_back(end_);
}

const std::vector<std::shared_ptr<Node> >&
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
