/*
 * MaxProduct.cpp
 *
 *  Created on: Apr 23, 2019
 *      Author: mirco
 */


#include "DAG/MaxProduct.h"

Eigen::VectorXf
maxProduct(const Eigen::MatrixXi& A, const Eigen::VectorXf& b)
{
	return (A.cast<float>().array().rowwise() * b.transpose().array()).rowwise().maxCoeff();
}

int
maxProductVector(const Eigen::VectorXi& a, const Eigen::VectorXf& b)
{
	return (a.cast<float>().array() * b.array()).maxCoeff();
}
