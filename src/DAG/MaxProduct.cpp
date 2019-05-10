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

Eigen::VectorXf
maxProductFloat(const Eigen::MatrixXf& A, const Eigen::VectorXf& b)
{
	return (A.array().rowwise() * b.transpose().array()).rowwise().maxCoeff();
}

int
maxProductVector(const Eigen::VectorXi& a, const Eigen::VectorXf& b)
{
	return (a.cast<float>().array() * b.array()).maxCoeff();
}

Eigen::MatrixXf
maxProductMatrix(const Eigen::MatrixXf& A, const Eigen::MatrixXf& B)
{
	Eigen::MatrixXf C(Eigen::MatrixXf::Zero(A.rows(), B.cols()));
	for (unsigned k = 0; k < B.cols(); k++)
	{
		C.col(k) = maxProductFloat(A, B.col(k));
	}
	return C;
}
