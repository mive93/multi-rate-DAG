#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <eigen3/Eigen/Core>
#include <VariableTaskSet/VariableTaskSet.h>
#include "MultiRate/MultiRateTaskset.h"
#include "Evaluation/Scheduling.h"
#include "Evaluation/Evaluation.h"
#include "Benchmark/DataFiles.h"
#include "Becker.h"
#include <ctime>

#define N_PERIODS 9

struct Task
{
public:
    int period_;
    float wcet_;
    float bcet_;
    float utilization_;

    void print()
    {
        std::cout << "Period: " << period_ << "\tWCET: " << wcet_ << "\tBCET: " << bcet_ << "\tU: " << utilization_ << std::endl;
    }
};

class WatersChallenge2015
{
private:
    const std::vector<int> periods_ = {1, 2, 5, 10, 20, 50, 100, 200, 1000}; //in ms
    const std::vector<int> p_shares_ = {0, 0, 0, 25, 25, 3, 20, 1, 0};
    // const std::vector<int> p_shares_ = {3, 2, 2, 25, 25, 3, 20, 1, 4};
    const std::vector<float> acet_ = {5.00, 4.20, 11.04, 10.09, 8.74, 17.56, 10.53, 2.56, 0.43}; //in us
    const std::vector<float> fmin_best_ = {0.19, 0.12, 0.17, 0.05, 0.11, 0.32, 0.09, 0.45, 0.68};
    const std::vector<float> fmax_best_ = {0.92, 0.89, 0.94, 0.99, 0.98, 0.95, 0.99, 0.98, 0.8};
    const std::vector<float> fmin_worst_ = {1.30, 1.54, 1.13, 1.06, 1.06, 1.13, 1.02, 1.03, 1.84};
    const std::vector<float> fmax_worst_ = {29.11, 19.04, 18.44, 30.03, 15.61, 7.76, 8.88, 4.90, 4.75};
    const std::vector<int> different_periods_in_chain_ = {1, 2, 3};
    const std::vector<int> dpic_shares_ = {70, 20, 10};
    const std::vector<int> same_period_in_chains_ = {2, 3, 4, 5};
    const std::vector<int> spic_shares_ = {30, 40, 20, 10};
    const std::vector<std::vector<float>> comunication_shares_ = {{0.015, 0, 0, 0.0025, 0.0025, 0, 0.0025, 0, 0},
                                                                  {0, 0.015, 0.0025, 0.0025, 0.0025, 0, 0.0025, 0, 0},
                                                                  {0, 0.0025, 0.15, 0.15, 0.015, 0.015, 0.0025, 0, 0},
                                                                  {0.015, 0.015, 0.015, 0.75, 0.15, 0.015, 0.15, 0.015, 0.0375},
                                                                  {0.0025, 0.0025, 0.0025, 0.15, 0.75, 0.015, 0.15, 0.0025, 0.015},
                                                                  {0, 0, 0.015, 0.015, 0.015, 0.0375, 0.0025, 0, 0},
                                                                  {0.0025, 0.0025, 0.0025, 0.375, 0.15, 0.015, 0.75, 0.015, 0.0375},
                                                                  {0, 0, 0, 0.0025, 0.0025, 0, 0.0025, 0.0025, 0.0025},
                                                                  {0, 0, 0, 0.0375, 0.015, 0, 0.0375, 0.0025, 0.15}};

    std::default_random_engine generator_;
    std::discrete_distribution<int> *p_distribution_;
    std::discrete_distribution<int> *spic_distribution_;
    std::discrete_distribution<int> *dpic_distribution_;
    std::vector<std::discrete_distribution<int>> comunication_distribution_;
    int period_mult_ = 1000;

    std::vector<std::vector<int>> chains_;
    std::vector<Task> taskset_;
    Eigen::MatrixXf edges_;
    int n_cores_;

    std::vector<std::pair<int, int>> genIndexPerPeriods(std::vector<Task> &taskset);
    int getRandomTaskIndexGivenPeriodIndex(std::vector<std::pair<int, int>> ipp, int p_index);

public:
    typedef enum
    {
        ONLY_N,
        ONLY_U,
        N_AND_U,
        TEST_1,
        TEST_2
    } task_gen_t;

    WatersChallenge2015(const int n_tasks, const int n_chains, const float utilization, task_gen_t generation, DataFiles &f, const int n_cores);
    bool SoAcomparison(DataFiles &f);
    std::vector<std::vector<int>> generateChains(std::vector<Task> taskset, int n_chains);
    std::vector<Task> generateTasksetGivenU(float utilization);
    std::vector<Task> generateTasksetGivenN(int max_n);
    std::vector<Task> generateTasksetUUnifast(int max_n, float utilization);
    Task generateTask(float util = -1);
    ~WatersChallenge2015();
};

bool compareTasksPeriod(const Task &t1, const Task &t2);
template <typename T>
float randMinMax(T min_val, T max_val);
std::vector<float> UUnifast(int ntasks, float utilization);

#endif /*BENCHMARK_H*/