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
	dagMatrix_ = dagMatrix_ - (dagMatrix_ * dagMatrix_ * ancestors_.transpose());
	convertToBooleanMat(dagMatrix_);
	fromDAGMatrix(dagMatrix_);
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

	std::map<unsigned int, int> n_instances;

	//figuring out number of rows and columns of the matrix
	for (auto node : nodes_)
		n_instances[node->groupId]++;

	std::map<unsigned int, int>::iterator max_inst =
			std::max_element(n_instances.begin(), n_instances.end(),
					[] (const std::pair<unsigned int,int>& a, const std::pair<unsigned int,int>& b)->bool
					{	return a.second < b.second;});

	//actually inserting the nodes
	y = n_instances.size() / 2 * distance;
	x = 0;
	cur_groupId = -1;

	float l_max = distance * (max_inst->second + 1);
	float cur_distance;

	for (auto node : nodes_)
	{
		if (node->name == "start")
		{
			tikz_file << "\\node[state, fill,draw=none,green, text=black] (" << node->uniqueId
					<< ") at (0,0) {" << node->shortName << "};\n";
			x = distance;
		}
		else if (node->name == "end")
		{
			tikz_file << "\\node[state, fill,draw=none,red,text=black](" << node->uniqueId
					<< ") at (" << max_inst->second * distance + distance << ",0) {"
					<< node->shortName << "};\n";
		}
		else
		{

			x += cur_distance;
			if (static_cast<int>(node->groupId) != cur_groupId)
			{
				cur_distance = l_max / (n_instances[node->groupId]);
				x = cur_distance / 2;
				y -= distance;
			}
			cur_groupId = node->groupId;

			if (node->groupId == 666)
				tikz_file << "\\node[state, fill,draw=none,blue,text=white] (" << node->uniqueId
						<< ") at (" << x << "," << y << ") {" << node->shortName << "};\n";
			else
				tikz_file << "\\node[state] (" << node->uniqueId << ") at (" << x << "," << y
						<< ") {$" << node->shortName << "$};\n";
		}
	}

	//inserting edges
	tikz_file << "\\path[->] \n";
	for (const auto& edge : edges_)
		tikz_file << "(" << edge.from->uniqueId << ") edge node {} (" << edge.to->uniqueId
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
		mat.coeffRef(edge.to->uniqueId, edge.from->uniqueId) = 1;
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
	dagMatrix_ = mat;
}

void
DAG::createMats()
{
	dagMatrix_ = toDAGMatrix();
	//Delete double edges
	fromDAGMatrix(dagMatrix_);

	ancestors_ =
			(dagMatrix_ + DAGMatrix::Identity(dagMatrix_.rows(), dagMatrix_.cols())).transpose();

	int n = ancestors_.rows();

	for (int k = 0; k < std::ceil(std::log2(static_cast<double>(n))); k++)
	{
		ancestors_ = ancestors_ * ancestors_;
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
		dagMatrix_(other.getDAGMatrix()), ancestors_(other.getAncestors()), definitelySerialized_(
				other.getDefinitelySerialized()), nodes_(other.getNodes()), edges_(
				other.getEdges()), nodeInfo_(other.nodeInfo_), start_(other.getStart()), end_(
				other.getEnd()), period_(other.period_)
{
}

bool
DAG::checkLongestChain() const
{
	unsigned n = nodes_.size();

	Eigen::VectorXi v = Eigen::VectorXi::Zero(n);
	Eigen::VectorXf val = Eigen::VectorXf::Zero(n);
	Eigen::VectorXf wc = Eigen::VectorXf::Zero(n);
	v[0] = 1;

	for (unsigned k = 0; k < n; ++k)
		wc[k] = nodes_[k]->wcet;

	unsigned endIdx = end_->uniqueId;

	for (unsigned k = 0; k < n; ++k)
	{
		v = dagMatrix_ * v;
		convertToBooleanVec(v);

		Eigen::VectorXf temp = wc.array() * v.cast<float>().array();
		val = maxProduct(dagMatrix_, val) + temp;

		if (val[endIdx] > static_cast<int>(period_))
			return false;

		if (v.isZero())
			break;
	}
	return true;
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
		if (k > N) // ignore nodes with higher group id
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
	Eigen::VectorXf val = Eigen::VectorXf::Zero(n);
	Eigen::VectorXi vBackwards = Eigen::VectorXi::Zero(n);
	Eigen::VectorXf valBackwards = Eigen::VectorXf::Zero(n);
	nodeInfo_.bc = Eigen::VectorXf::Zero(n);
	nodeInfo_.wc = Eigen::VectorXf::Zero(n);
	v[0] = 1;
	vBackwards[1] = 1;

	auto back = dagMatrix_.transpose();

	for (unsigned k = 0; k < n; ++k)
	{
		nodeInfo_.bc[k] = nodes_[k]->bcet;
		nodeInfo_.wc[k] = nodes_[k]->wcet;
	}

	for (unsigned k = 0; k < n; ++k)
	{
		v = dagMatrix_ * v;
		vBackwards = back * vBackwards;
		convertToBooleanVec(v);
		convertToBooleanVec(vBackwards);

		Eigen::VectorXf temp = nodeInfo_.bc.array() * v.cast<float>().array();
		// val = max_cellwise(val, val_after_step)
		val = (maxProduct(dagMatrix_, val) + temp).array().max(val.array()).matrix();

		Eigen::VectorXf tempBack = nodeInfo_.wc.array() * vBackwards.cast<float>().array();
		// val = max_cellwise(val, val_after_step)
		valBackwards =
				(maxProduct(back, valBackwards) + tempBack).array().max(valBackwards.array()).matrix();

		if (v.isZero())
			break;
	}

	nodeInfo_.est = val - nodeInfo_.bc;
	nodeInfo_.lst = (period_ - valBackwards.array()).matrix();
	nodeInfo_.eft = nodeInfo_.est + nodeInfo_.bc;
	nodeInfo_.lft = nodeInfo_.lst + nodeInfo_.wc;

	definitelySerialized_ = ancestors_.transpose() - DAGMatrix::Identity(n, n);

	for (unsigned k = 0; k < n; k++)
	{
		for (unsigned l = 0; l < n; l++)
		{
			if (nodeInfo_.est[k] >= nodeInfo_.lft[l])
				definitelySerialized_.coeffRef(k, l) = 1;
		}
	}

}

std::ostream &
operator <<(std::ostream &out, const DAG::NodeInfo &info)
{
	out << "BC: " << info.bc.transpose() << std::endl;
	out << "WC: " << info.wc.transpose() << std::endl << std::endl;

	out << "EST: " << info.est.transpose() << std::endl;
	out << "LST: " << info.lst.transpose() << std::endl;
	out << "EFT: " << info.eft.transpose() << std::endl;
	out << "LFT: " << info.lft.transpose() << std::endl;
	return out;
}

LatencyInfo
DAG::getLatencyInfo(std::vector<unsigned> dataChain)
{
	unsigned n = nodes_.size();

	unsigned maxGroup = 0;
	for (const auto& g : dataChain)
		if (g > maxGroup)
			maxGroup = g;

	unsigned startGroup = dataChain.front();
	unsigned endGroup = dataChain.back();

	std::vector<unsigned> groupChain(std::next(dataChain.begin()), std::prev(dataChain.end()));

	auto groupMat = getGroupMatrix(maxGroup + 1);

	DAGMatrix A = definitelySerialized_;
	DAGMatrix B = DAGMatrix::Ones(n, n);
	DAGMatrix C = DAGMatrix::Ones(n, n);

	unsigned hpCounter = 0;

	for (auto g : groupChain)
	{
		auto diag = groupMat.col(g).asDiagonal();
		DAGMatrix Anew = definitelySerialized_ * diag * A;
		if (Anew.isZero())
		{
			A = B;
			B = DAGMatrix::Ones(n, n);
			Anew = definitelySerialized_ * diag * A;
			hpCounter++;
		}
		B = DAGMatrix::Ones(n, n) * diag * A + definitelySerialized_ * diag * B;
		A = Anew;
		convertToBooleanMat(A);
		convertToBooleanMat(B);
	}

	A = groupMat.col(endGroup).asDiagonal() * A;
	B = groupMat.col(endGroup).asDiagonal() * B;
	C = groupMat.col(endGroup).asDiagonal() * C;

	convertToBooleanMat(A);
	convertToBooleanMat(B);
	convertToBooleanMat(C);
	std::vector<unsigned> starters;
	for (unsigned k = 0; k < n; k++)
		if (groupMat.col(startGroup)[k] == 1)
			starters.push_back(k);


	LatencyInfo info;
	info.reactionTime = 0;
	info.maxLatency = 0;
	info.minLatency = 0; // Not implemented yet

	for (auto it = starters.begin(); it != starters.end(); it++)
	{
		Eigen::VectorXi temp;
		temp.resize(3 * n);
		temp << A.col(*it), B.col(*it), C.col(*it);

		Eigen::VectorXi tempNext;
		tempNext.resize(3 * n);

		auto next = std::next(it);
		if (next == starters.end())
		{
			tempNext << Eigen::VectorXi::Zero(n, 1), A.col(starters.front()), B.col(
					starters.front());
		}
		else
		{
			tempNext << A.col(*next), B.col(*next), C.col(*next);
		}

		unsigned first = 0;
		for (; first < 3 * n; first++)
			if (temp[first] == 1)
				break;

		float diff = (hpCounter + (first / n)) * period_ + nodeInfo_.lft[first % n]
				- nodeInfo_.est[*it];
		if (diff > info.reactionTime)
		{
			info.reactionTime = diff;
			info.reactionTimePair = std::make_pair(*it, first % n);
		}

		temp -= tempNext;
		convertToBooleanVec(temp);
		if (temp.isZero())
			continue;

		unsigned last = 3 * n - 1;
		for (; last >= 0; last--)
			if (temp[last] == 1)
				break;

		diff = (hpCounter + (last / n)) * period_ + nodeInfo_.lft[last % n] - nodeInfo_.est[*it];
		if (diff > info.maxLatency)
		{
			info.maxLatency = diff;
			info.maxLatencyPair = std::make_pair(*it, last % n);
		}
	}
	return info;
}
