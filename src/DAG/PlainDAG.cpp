/*
 * PlainDAG.cpp
 *
 *  Created on: May 8, 2019
 *      Author: mirco
 */
#include <DAG/PlainDAG.h>
#include <iostream>

PlainDAG::PlainDAG(const DAG& dag, unsigned N) :
		syncTimes(dag.getSyncTimes()), period(dag.getPeriod())
{
	const auto& dagMatFull = dag.getDAGMatrix();
	unsigned n = dagMatFull.rows();
	unsigned nNew = n - 2 - (syncTimes.size() * 2 + 1);

	dagMatrix = dagMatFull.block(2, 2, nNew, nNew).cast<bool>();

	syncMatrixOffset = dagMatFull.block(2, 2 + nNew, nNew, syncTimes.size()).cast<bool>();

	syncMatrixDeadline = dagMatFull.block(2 + nNew, 2, syncTimes.size(), nNew).cast<bool>();

	groupMatrix = dag.getGroupMatrix(N).cast<bool>().block(2, 0, nNew, N);

	const auto& info = dag.getNodeInfo();
	nodeInfo.bc = info.bc.bottomRows(n - 2).topRows(nNew);
	nodeInfo.wc = info.wc.bottomRows(n - 2).topRows(nNew);
	nodeInfo.est = info.est.bottomRows(n - 2).topRows(nNew);
	nodeInfo.lst = info.lst.bottomRows(n - 2).topRows(nNew);
	nodeInfo.eft = info.eft.bottomRows(n - 2).topRows(nNew);
	nodeInfo.lft = info.lft.bottomRows(n - 2).topRows(nNew);
}

LatencyInfo
PlainDAG::getLatencyInfo(const std::vector<unsigned>& chain)
{
	unsigned n = dagMatrix.rows();
	BoolMatrix serialized = dagMatrix || BoolMatrix::Identity(n, n)
			|| (syncMatrixOffset * syncMatrixDeadline);

	for (int k = 0; k < std::log2((float)n); k++)
	{
		serialized *= serialized;
	}

	serialized -= BoolMatrix::Identity(n,n);
	for (unsigned k = 0; k < n; k++)
	{
		for (unsigned l = 0; l < n; l++)
		{
			if (nodeInfo.est[k] >= nodeInfo.lft[l])
				serialized.coeffRef(k, l) = 1;
		}
	}



}
