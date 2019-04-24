/*
 * MaxProduct.h
 *
 *  Created on: Apr 23, 2019
 *      Author: mirco
 */

#ifndef DAG_MAXPRODUCT_H_
#define DAG_MAXPRODUCT_H_

#include <eigen3/Eigen/Dense>

Eigen::VectorXi
maxProduct(const Eigen::MatrixXi& A, const Eigen::VectorXi& b);

int
maxProductVector(const Eigen::VectorXi& a, const Eigen::VectorXi& b);



#endif /* DAG_MAXPRODUCT_H_ */
