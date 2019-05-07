/*
 * PlainDAG.h
 *
 *  Created on: May 7, 2019
 *      Author: mirco
 */

#ifndef DAG_PLAINDAG_H_
#define DAG_PLAINDAG_H_
#include "DAG/DAG.h"
#include <eigen3/Eigen/Core>
#include <vector>

struct PlainDAG
{

	PlainDAG(const DAG& dag, unsigned N);

	Eigen::Matrix<bool, -1, -1> dagMatrix;
	Eigen::Matrix<bool, -1, -1> groupMatrix;
	Eigen::Matrix<bool, -1, -1> syncMatrix;

	std::vector<float> syncTimes;

};

namespace dp
{
template <class Archive, typename Type>
inline void
serialize(Archive& ar, PlainDAG& dag)
{

}
}



#endif /* DAG_PLAINDAG_H_ */
