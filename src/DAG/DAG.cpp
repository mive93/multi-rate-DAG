/*
 * dag.cpp
 *
 *  Created on: Apr 1, 2019
 *      Author: mirco
 */
#include <DAG/IntervalPropagation.h>
#include "DAG/MaxProduct.h"
#include "MultiRate/MultiRateTaskset.h"
#include "DAG/DAG.h"
#include <cmath>
#include <eigen3/Eigen/Core>

#include <eigen3/Eigen/Dense>
#include <iomanip>

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
						<< ") {$" << node->uniqueId << "$};\n";
		}
	}

	//inserting edges
	tikz_file << "\\path[->] \n";
	for (const auto& edge : edges_)
		tikz_file << "(" << edge.from->uniqueId << ") edge node {} (" << edge.to->uniqueId << ")\n";
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
	start_->groupId = 668;
	start_->name = "start";
	start_->shortName = "S";

	end_ = std::make_shared<Node>();
	end_->bcet = 0;
	end_->wcet = 0;
	end_->deadline = period_;
	end_->offset = period_;
	end_->uniqueId = 10000;
	end_->groupId = 668;
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
				other.getDefinitelySerialized()), partiallySerialized_(
				other.getPartiallySerialized()), partiallySerializedReact_(
				other.getPartiallySerializedReact()), partiallySerializedReactBInit_(
				other.getPartiallySerializedReactBInit()), nodes_(other.getNodes()), edges_(
				other.getEdges()), nodeInfo_(other.nodeInfo_), start_(other.getStart()), end_(
				other.getEnd()), period_(other.period_), originatingTaskset_(
				other.getOriginatingTaskset())
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

	partiallySerialized_ = definitelySerialized_.cast<float>();

	const auto& nI = nodeInfo_;

	for (unsigned from = 0; from < n; from++)
	{
		for (unsigned to = 0; to < n; to++)
		{
			if (definitelySerialized_.coeff(to, from) == 1)
				continue;

			if (nI.lft[from] > nI.est[to] && nI.lft[from] < nI.lst[to])
			{
				float aw = (nI.lst[to] - nI.lft[from]) / (nI.lft[to] - nI.eft[to]);
				partiallySerialized_.coeffRef(to, from) = aw;
			}
		}
	}

	partiallySerializedReactBInit_ = DAGMatrixFloat::Zero(n, n);
	partiallySerializedReact_ = definitelySerialized_.cast<float>();
	for (unsigned from = 2; from < n; from++)
	{
		for (unsigned to = 2; to < n; to++)
		{
			if (nodes_.at(to)->groupId != nodes_.at(to - 1)->groupId)
			{
				if (nodes_.at(to - 1)->groupId >= 666)
					continue;

				if (dagMatrix_.coeff(to - 1, from) == 1)
					continue;

				float ar = (nI.lft[from] - nI.est[to - 1]);
				if (nI.lst[from] - nI.est[from] != 0)
					ar /= (nI.lst[from] - nI.est[from]);
				unsigned firstInstance = originatingTaskset_->getNodes()[nodes_.at(to - 1)->groupId
						- 1]->nodes.front()->uniqueId;
				partiallySerializedReactBInit_.coeffRef(firstInstance, from) = std::max(
						std::min(ar, 1.0f), 0.0f);
				continue;
			}

			if (nI.eft[from] < nI.est[to])
			{
				float ar = (nI.lft[from] - nI.est[to - 1]);

				if (nI.lst[from] - nI.est[from] > 0)
					ar /= (nI.lst[from] - nI.est[from]);
				partiallySerializedReact_.coeffRef(to, from) = std::max(std::min(ar, 1.0f), 0.0f);
			}
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
DAG::getLatencyInfo(const std::vector<unsigned>& dataChain) const
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

	DAGMatrixFloat Af;
	DAGMatrixFloat Bf;
	if (groupChain.empty())
	{
		Af = partiallySerialized_;
		Bf = DAGMatrixFloat::Ones(n, n);
	}

	DAGMatrixFloat definitelySerializedNextOnly = definitelySerialized_.cast<float>();
	for (unsigned from = 0; from < n; from++)
	{
		for (unsigned to = 1; to < n; to++)
		{
			if (nodes_[to]->groupId != nodes_[to - 1]->groupId)
				continue;

			if (definitelySerialized_.coeff(to - 1, from) == 1)
				definitelySerializedNextOnly.coeffRef(to, from) = 0;
		}
	}

	DAGMatrixFloat BInit = (partiallySerializedReactBInit_.array() > 0).matrix().cast<float>();

	DAGMatrixFloat AfReact = partiallySerializedReact_;
	DAGMatrixFloat BfReact = partiallySerializedReactBInit_;

	unsigned hpCounter = 0;
	unsigned hpCounterReact = 0;

	for (auto g : groupChain)
	{
		Af = A.cast<float>();
		Bf = B.cast<float>();
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

	for (auto g : groupChain)
	{
		auto diag = groupMat.cast<float>().col(g).asDiagonal();
		DAGMatrixFloat Anew = maxProductMatrix(definitelySerializedNextOnly * diag, AfReact);
		if (Anew.isZero(1e-6))
		{
			AfReact = BfReact;
			BfReact = BInit;
			Anew = maxProductMatrix(definitelySerializedNextOnly * diag, AfReact);
			hpCounterReact++;
		}
		BfReact = maxProductMatrix(BInit * diag, AfReact).array().max(
				maxProductMatrix(definitelySerializedNextOnly * diag, BfReact).array()).matrix();
		AfReact = Anew;
	}

	if (!groupChain.empty())
	{
		auto diag = groupMat.cast<float>().col(groupChain.back()).asDiagonal();
		DAGMatrixFloat Anew = maxProductMatrix(partiallySerialized_ * diag, Af);
		if (Anew.isZero(1e-6))
		{
			Af = Bf;
			Bf = DAGMatrixFloat::Ones(n, n);
			Anew = maxProductMatrix(partiallySerialized_ * diag, Af);
		}
		Bf = maxProductMatrix(DAGMatrixFloat::Ones(n, n) * diag, Af).array().max(
				maxProductMatrix(partiallySerialized_ * diag, Bf).array());
		Af = Anew;
	}

	A = groupMat.col(endGroup).asDiagonal() * A;
	B = groupMat.col(endGroup).asDiagonal() * B;
	C = groupMat.col(endGroup).asDiagonal() * C;

	Af = groupMat.cast<float>().col(endGroup).asDiagonal() * Af;
	Bf = groupMat.cast<float>().col(endGroup).asDiagonal() * Bf;

	AfReact = groupMat.cast<float>().col(endGroup).asDiagonal() * AfReact;
	BfReact = groupMat.cast<float>().col(endGroup).asDiagonal() * BfReact;

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
		Eigen::VectorXf temp;
		temp.resize(3 * n);
		temp << A.cast<float>().col(*it), B.cast<float>().col(*it), C.cast<float>().col(*it);

		Eigen::VectorXf tempReact;
		tempReact.resize(2 * n);
		tempReact << AfReact.col(*it), BfReact.col(*it);

		Eigen::VectorXf tempNext;
		tempNext.resize(3 * n);

		auto next = std::next(it);
		if (next == starters.end())
		{
			tempNext << Eigen::VectorXf::Zero(n, 1), Af.col(starters.front()), Bf.col(
					starters.front());
		}
		else
		{
			tempNext << Af.col(*next), Bf.col(*next), C.cast<float>().col(*next);
		}

		//Reaction time
		for (unsigned r = 0; r < 2 * n; r++)
		{
			if (tempReact[r] > 1e-5)
			{
				float diff = (hpCounterReact + (r / n)) * period_ + nodeInfo_.lft[r % n]
						- (nodeInfo_.est[*it]
								+ (1 - tempReact[r]) * (nodeInfo_.lst[*it] - nodeInfo_.est[*it]));
				if (diff > info.reactionTime)
				{
					info.reactionTime = diff;
					info.reactionTimePair = std::make_pair(*it, r % n);
				}
			}
		}

		//Data Age
		temp -= tempNext;
		temp = ((temp.array() > 0).matrix().cast<float>().array() * temp.array()).matrix();
		if (temp.isZero())
			continue;

		unsigned last = 3 * n - 1;
		for (; last >= 0; last--)
			if (temp[last] > 0)
				break;

		float val = temp[last] * (nodeInfo_.lft[last % n] - nodeInfo_.eft[last % n]);

		float diff = (hpCounter + (last / n)) * period_ + nodeInfo_.eft[last % n] + val
				- nodeInfo_.est[*it];
		if (diff > info.maxLatency)
		{
			info.maxLatency = diff;
			info.maxLatencyPair = std::make_pair(*it, last % n);
		}
	}

	getMinLatency(dataChain, info);
	return info;
}


LatencyInfo
DAG::getMinLatency(const std::vector<unsigned>& dataChain, LatencyInfo& latency) const
{

	unsigned maxGroup = 0;
	for (const auto& g : dataChain)
		if (g > maxGroup)
			maxGroup = g;

	std::vector<unsigned> groupChain(dataChain.begin(), std::prev(dataChain.end()));
	std::reverse(groupChain.begin(), groupChain.end());

	auto groupMat = getGroupMatrix(maxGroup + 1);

	std::vector<unsigned> starters = getIdsFromGroup(groupMat, dataChain.back());

	const auto& info = nodeInfo_;
	latency.minLatency = std::numeric_limits<float>::max();
	for (auto starter : starters)
	{
		IntervalPropagation prop;
		prop.startInterval = std::make_pair(nodeInfo_.eft[starter], nodeInfo_.lft[starter]);
		prop.interval = prop.startInterval;


		prop.shiftWriteRead(info.bc[starter], info.lst[starter]);
		unsigned currentReadNode = starter;

		for (auto group : groupChain)
		{
			auto nodes = getIdsFromGroup(groupMat, group);
			std::reverse(nodes.begin(), nodes.end());

			bool found = false;
			for (auto node : nodes)
			{
				//Exclude if precedence from currentReadNode to node
				if (dagMatrix_.coeff(node, currentReadNode) == 1 || node == currentReadNode)
					continue;
				if (!prop.canReact(info.eft[node], info.lft[node]))
					continue;


				prop.react(info.eft[node], info.lft[node]);
				prop.shiftWriteRead(info.bc[node], info.lst[node]);
				currentReadNode = node;
				found = true;
				break;
			}

			if (!found)
			{
				prop.addHyperPeriod(period_);
				for (auto node : nodes)
				{
					if (!prop.canReact(info.eft[node], info.lft[node]))
						continue;

					prop.react(info.eft[node], info.lft[node]);
					prop.shiftWriteRead(info.bc[node], info.lst[node]);
					currentReadNode = node;
					break;
				}
			}

		}


		float diff = prop.startInterval.second - prop.interval.second;
		if (diff < latency.minLatency)
		{
			latency.minLatency = diff;
			latency.minLatencyPair = std::make_pair(currentReadNode, starter);
		}

	}

	return latency;

}

std::vector<unsigned>
DAG::getIdsFromGroup(const DAGMatrix& groupMat, unsigned group) const
{
	std::vector<unsigned> ids;
	for (unsigned k = 0; k < groupMat.rows(); k++)
	{
		if (groupMat.col(group)[k] == 1)
			ids.push_back(k);
	}
	return ids;
}

