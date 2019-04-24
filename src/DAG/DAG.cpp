/*
 * dag.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */
#include <DAG/MaxProduct.h>
#include "DAG/DAG.h"
#include <cmath>
#include <eigen3/Eigen/Core>

#include <eigen3/Eigen/Dense>

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
	step[0] = true;

	for (unsigned k = 0; k < n; ++k)
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
	/*
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
	 */

	DAGMatrix mat = dagMatrix_ - (dagMatrix_ * dagMatrix_ * ancestors_.transpose());
	convertToBooleanMat(mat);
	fromDAGMatrix(mat);
	dagMatrix_ = toDAGMatrix();

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
	tikz_file << "\\documentclass[tikz,border=10pt]{standalone}\n"
				"\\usepackage{tkz-graph}\n"
				"\\usetikzlibrary{automata}\n"
				"\\usetikzlibrary[automata]\n"
				"\\begin{document}\n";

	tikz_file << "\\begin{tikzpicture}[shorten >=1pt,node distance=3cm,auto,bend angle=45]\n";

	const int distance = 2; //distance between the nodes
	float x = 0, y = 0; //x and y of the nodes
	int cur_groupId = -1;

	std::map<unsigned int,int> n_instances;
	
	//figuring out number of rows and columns of the matrix
	for (auto node : nodes_)	
		n_instances[node->groupId]++;

	std::map<unsigned int,int>::iterator max_inst
        = std::max_element(n_instances.begin(),n_instances.end(),[] (const std::pair<unsigned int,int>& a, const std::pair<unsigned int,int>& b)->bool{ return a.second < b.second; } );

	//actually inserting the nodes
	y = (n_instances.size() -1) /2;
	x = 0;
	cur_groupId = -1;

	for (auto node : nodes_)
	{
		if (node->name == "start")
		{
			tikz_file << "\\node[state, fill,draw=none,green, text=black] (" << node->shortName
					<< ") at (0,0) {" << node->shortName << "};\n";
			x = distance;
		}
		else if (node->name == "end")
		{
			tikz_file << "\\node[state, fill,draw=none,red,text=black](" << node->shortName
					<< ") at (" << max_inst->second*distance + distance << ",0) {" << node->shortName << "};\n";
		}
		else
		{
			x += distance;
			if (static_cast<int>(node->groupId) != cur_groupId)
			{
				x = (max_inst->second/2.0 - n_instances[node->groupId]/2.0 +1 )*distance;
				y -= distance;
			}
			cur_groupId = node->groupId;

			if (node->groupId == 666)
				tikz_file << "\\node[state, fill,draw=none,blue,text=white] (" << node->shortName
						<< ") at (" << x << "," << y << ") {" << node->shortName << "};\n";
			else
				tikz_file << "\\node[state] (" << node->shortName << ") at (" << x << "," << y
						<< ") {$" << node->shortName << "$};\n";
		}
	}

	//inserting edges
	tikz_file << "\\path[->] \n";
	for (const auto& edge : edges_)
		tikz_file << "(" << edge.from->shortName << ") edge node {} (" << edge.to->shortName
				<< ")\n";
	tikz_file << ";\n";

	//ending tikz figure
	tikz_file << "\\end{tikzpicture}\n";
	tikz_file << "\\end{document}\n";

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

void
DAG::fromDAGMatrix(const DAGMatrix& mat)
{
	edges_.clear();
	for (int k = 0; k < mat.cols(); k++)
	{
		for (int l = 0; l < mat.rows(); l++)
		{
			if (mat.coeff(l, k) == 1)
				edges_.push_back(Edge(nodes_.at(k), nodes_.at(l)));
		}
	}
}

void
DAG::createMats()
{
	dagMatrix_ = toDAGMatrix();
	//Delete double edges
	fromDAGMatrix(dagMatrix_);

//	descendents_ = dagMatrix_ + DAGMatrix::Identity(dagMatrix_.rows(), dagMatrix_.cols());
	ancestors_ =
			(dagMatrix_ + DAGMatrix::Identity(dagMatrix_.rows(), dagMatrix_.cols())).transpose();

	int n = ancestors_.rows();

	for (int k = 0; k < std::ceil(std::log2(static_cast<double>(n))); k++)
	{
//		descendents_ = descendents_ * descendents_;
		ancestors_ = ancestors_ * ancestors_;
//		convertToBooleanMat(descendents_);
		convertToBooleanMat(ancestors_);
	}

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
		dagMatrix_(other.getDAGMatrix()), ancestors_(other.getAncestors()), nodes_(
				other.getNodes()), edges_(other.getEdges()), start_(other.getStart()), end_(
				other.getEnd()), period_(other.period_)
{
}

bool
DAG::checkLongestChain() const
{
	/*
	 std::vector<std::vector<int>> children;

	 for (int k = 0; k < dagMatrix_.cols(); ++k)
	 {
	 children.push_back(std::vector<int>());
	 for (int l = 0; l < dagMatrix_.rows(); ++l)
	 {
	 if (dagMatrix_.col(k)[l] == 1)
	 children[k].push_back(l);
	 }
	 }

	 Chain chain;
	 chain.nodesStack.push_back(0);

	 try
	 {
	 chainRecursionWCET(chain, children);
	 } catch (int&)
	 {
	 return false;
	 }

	 return true;
	 */

	unsigned n = nodes_.size();

	Eigen::VectorXi v = Eigen::VectorXi::Zero(n);
	Eigen::VectorXi val = Eigen::VectorXi::Zero(n);
	Eigen::VectorXi wc = Eigen::VectorXi::Zero(n);
	v[0] = 1;

	for (unsigned k = 0; k < n; ++k)
		wc[k] = nodes_[k]->wcet;

	unsigned endIdx = end_->uniqueId;

	for (unsigned k = 0; k < n; ++k)
	{
		v = dagMatrix_ * v;
		convertToBooleanVec(v);

		Eigen::VectorXi temp = wc.array() * v.array();
		val = maxProduct(dagMatrix_, val) + temp;

		if (val[endIdx] > period_)
			return false;

		if (v.isZero())
			break;
	}
	return true;
}

void
DAG::chainRecursionWCET(Chain& chain, const std::vector<std::vector<int> >& children) const
{
	auto node = chain.nodesStack.back();
	for (auto child : children[node])
	{
		chain.nodesStack.push_back(child);
		chain.wcetsStack.push_back(nodes_[child]->wcet);
		chain.wcet += chain.wcetsStack.back();
		if (chain.wcet > period_)
			throw(int(1));
		chainRecursionWCET(chain, children);
	}

	chain.wcet -= chain.wcetsStack.back();
	chain.wcetsStack.pop_back();
	chain.nodesStack.pop_back();
}

DAG::DAGMatrix
DAG::getJitterMatrix() const
{
	DAGMatrix ret = DAGMatrix::Ones(ancestors_.rows(), ancestors_.cols()) - ancestors_
			- ancestors_.transpose();
	convertToBooleanMat(ret);

	return ret;
}

Eigen::MatrixXi
DAG::getGroupMatrix(int N) const
{
	int n = nodes_.size();
	Eigen::Matrix<int, -1, -1> mat = Eigen::Matrix<int, -1, -1>::Zero(n, N);

	for (auto node : nodes_)
	{
		int k = node->groupId;
		if (k == 0 || k == 666 || k == 667) //Start and end
			continue;
		mat.coeffRef(node->uniqueId, k - 1) = 1;
	}

	return mat;
}

void
DAG::convertToBooleanMat(DAGMatrix& mat) const
{
	mat = (mat.array() > 0).matrix().cast<int>();
}

void
DAG::convertToBooleanVec(Eigen::VectorXi& vec) const
{
	vec = (vec.array() > 0).matrix().cast<int>();
}

void
DAG::createNodeInfo()
{
	unsigned n = nodes_.size();

	Eigen::VectorXi v = Eigen::VectorXi::Zero(n);
	Eigen::VectorXi val = Eigen::VectorXi::Zero(n);
	Eigen::VectorXi vBackwards = Eigen::VectorXi::Zero(n);
	Eigen::VectorXi valBackwards = Eigen::VectorXi::Zero(n);
	Eigen::VectorXi bc = Eigen::VectorXi::Zero(n);
	v[0] = 1;
	vBackwards[1] = 1;

	auto back = dagMatrix_.transpose();

	for (unsigned k = 0; k < n; ++k)
		bc[k] = nodes_[k]->bcet;

	for (unsigned k = 0; k < n; ++k)
	{
		v = dagMatrix_ * v;
		vBackwards = back * vBackwards;
		convertToBooleanVec(v);
		convertToBooleanVec(vBackwards);

		Eigen::VectorXi temp = bc.array() * v.array();
		// val = max_cellwise(val, val_after_step)
		val = (maxProduct(dagMatrix_, val) + temp).array().max(val.array()).matrix();

		Eigen::VectorXi tempBack = bc.array() * vBackwards.array();
		// val = max_cellwise(val, val_after_step)
		valBackwards = (maxProduct(back, valBackwards) + tempBack).array().max(valBackwards.array()).matrix();

		if (v.isZero())
			break;
	}

	std::cout << val - bc << std::endl << std::endl;
	std::cout << (period_ - valBackwards.array()) << std::endl;

	for (unsigned k = 0; k < n; ++k)
	{

	}
}
























