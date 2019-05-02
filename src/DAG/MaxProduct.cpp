/*
 * MaxProduct.cpp
 *
 *  Created on: Apr 23, 2019
 *      Author: mirco
 */


#include "DAG/MaxProduct.h"

Eigen::VectorXi
maxProduct(const Eigen::MatrixXi& A, const Eigen::VectorXi& b)
{
	return (A.array().rowwise() * b.transpose().array()).rowwise().maxCoeff();
}

int
maxProductVector(const Eigen::VectorXi& a, const Eigen::VectorXi& b)
{
	return (a.array() * b.array()).maxCoeff();
}
