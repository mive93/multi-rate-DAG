/*
 * PlainDAG.h
 *
 *  Created on: May 7, 2019
 *      Author: mirco
 */

#ifndef DAG_PLAINDAG_H_
#define DAG_PLAINDAG_H_
#include <uavAP/Core/DataPresentation/APDataPresentation/BasicSerialization.h>
#include "DAG/DAG.h"
#include <eigen3/Eigen/Core>
#include <iostream>
#include <vector>

struct PlainDAG
{
	using BoolMatrix = Eigen::Matrix<bool, -1, -1>;

	PlainDAG() = default;

	PlainDAG(const DAG& dag, unsigned N);

	LatencyInfo
	getLatencyInfo(const std::vector<unsigned>& chain);

	BoolMatrix dagMatrix;
	BoolMatrix groupMatrix;
	BoolMatrix syncMatrixOffset;
	BoolMatrix syncMatrixDeadline;

	std::vector<float> syncTimes;

	DAG::NodeInfo nodeInfo;

	unsigned period = 0;

};

namespace dp
{

template<typename T>
struct is_eigen_matrix: public std::false_type
{
};

template<typename T, int n, int m>
struct is_eigen_matrix<Eigen::Matrix<T, n, m>> : public std::true_type
{
};

template<class Archive, typename Type>
inline void
load(Archive& ar, typename std::enable_if<is_eigen_matrix<Type>::value, Type>::type& val)
{
	typename Type::Index rows, cols;
	ar & rows;
	ar & cols;

	val.resize(rows, cols);
	load(ar, reinterpret_cast<char*>(val.data()), val.size() * sizeof(typename Type::value_type));

}

template<class Archive, typename Type>
inline void
store(Archive& ar, typename std::enable_if<is_eigen_matrix<Type>::value, Type>::type& val)
{
	ar & val.rows();
	ar & val.cols();

	store(ar, reinterpret_cast<char*>(val.data()), val.size() * sizeof(typename Type::value_type));

}

template<class Archive, typename Type>
inline void
serialize(Archive& ar, typename std::enable_if<is_eigen_matrix<Type>::value, Type>::type& val)
{
	split(ar, val);
}

template<class Archive, typename Type>
inline void
serialize(Archive& ar, DAG::NodeInfo& info)
{
	ar & info.bc;
	ar & info.wc;
	ar & info.est;
	ar & info.lst;
	ar & info.eft;
	ar & info.lft;
}

template<class Archive, typename Type>
inline void
serialize(Archive& ar, PlainDAG& dag)
{
	ar & dag.dagMatrix;
	ar & dag.groupMatrix;
	ar & dag.syncMatrixOffset;
	ar & dag.syncMatrixDeadline;
	ar & dag.syncTimes;
	ar & dag.nodeInfo;
	ar & dag.period;
}
}

#endif /* DAG_PLAINDAG_H_ */
