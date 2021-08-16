/*
 * MaxProduct.h
 *
 *  Created on: Apr 23, 2019
 *      Author: mirco
 */

#ifndef DAG_MAXPRODUCT_H_
#define DAG_MAXPRODUCT_H_

#include <eigen3/Eigen/Dense>

Eigen::VectorXf
maxProduct(const Eigen::MatrixXi& A, const Eigen::VectorXf& b);


Eigen::VectorXf
maxProductFloat(const Eigen::MatrixXf& A, const Eigen::VectorXf& b);

int
maxProductVector(const Eigen::VectorXi& a, const Eigen::VectorXf& b);


Eigen::MatrixXf
maxProductMatrix(const Eigen::MatrixXf& A, const Eigen::MatrixXf& B);



#endif /* DAG_MAXPRODUCT_H_ */
