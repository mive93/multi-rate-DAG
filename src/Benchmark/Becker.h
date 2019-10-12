#ifndef BECKER_H
#define BECKER_H

#include <iostream>
#include "MultiRate/MultiRateTaskset.h"
#include "DPT.hpp"

class Becker
{
private:
    MultiRateTaskset t_;
    std::vector<std::shared_ptr<MultiNode>> mtasks_;
    int n_tasks_;
    int hyperperiod_;
    std::vector<std::vector<float>> Rmin;
    std::vector<std::vector<float>> Rmax;
    std::vector<std::vector<float>> Dmin;
    std::vector<std::vector<float>> Dmin_local;
    std::vector<std::vector<float>> Dmax;
    std::vector<JLD> dependencies;

public:
    Becker(const MultiRateTaskset &t, std::vector<std::shared_ptr<MultiNode>> mtasks);
    bool follows(int i, int j, int k, int l);
    float maxLatency(int i, int j, int k, int l);
    float minLatency(int i, int j, int k, int l, int z, int w);

    int computeL(int i, int j, int k);
    float computeChainMinMaxLatency(const std::vector<int> &chain);
    void buildPropTree(const std::vector<int> &chain, std::pair<int, int> &succ_root, std::vector<std::pair<int, int>> &leaves, const int i, const int j, int i_chain, DPT &tree);
    void computeIntervals(int hyperperiod_count);
    bool insertJobLevelDependency(int i, int j, int k, int l);
    bool synthesizeDependencies(const std::vector<int> &chain, float deadline);
    void initilizeDminLoc();

    void printIntervals();
    ~Becker();
};

#endif /*BECKER_H*/