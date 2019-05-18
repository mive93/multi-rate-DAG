/*
 * PlainTaskSet.h
 *
 *  Created on: May 18, 2019
 *      Author: mirco
 */

#ifndef MULTIRATE_PLAINTASKSET_H_
#define MULTIRATE_PLAINTASKSET_H_
#include <eigen3/Eigen/Core>
#include <vector>

struct PlainTaskSet
{
	Eigen::VectorXf wcet;
	Eigen::VectorXf bcet;

	std::vector<std::string> name;
};

inline std::ostream&
operator <<(std::ostream& o, const PlainTaskSet& set)
{
	for (unsigned k = 0; k < set.name.size(); k++)
		o << "Task " << k << ": " << set.name[k] << " [" << set.bcet[k] << ", " << set.wcet[k]
				<< "]" << std::endl;
	return o;
}

namespace dp
{
template<class Archive, typename >
inline void
serialize(Archive& ar, PlainTaskSet& plain)
{
	ar & plain.wcet;
	ar & plain.bcet;
	ar & plain.name;
}

}

#endif /* MULTIRATE_PLAINTASKSET_H_ */
