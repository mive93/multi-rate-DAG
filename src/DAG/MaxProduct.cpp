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
	unsigned n = A.rows();
	Eigen::VectorXi res = Eigen::VectorXi::Zero(n, 1);

	for (unsigned k = 0; k < n; ++k)
	{
		res[k] = maxProductVector(A.row(k), b);
	}
	return res;
}

int
maxProductVector(const Eigen::VectorXi& a, const Eigen::VectorXi& b)
{
	return (a.array() * b.array()).maxCoeff();
}
