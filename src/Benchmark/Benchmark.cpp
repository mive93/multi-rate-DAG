#include "Benchmark.h"

bool compareTasksPeriod(const Task &t1, const Task &t2)
{
    return (t1.period_ < t2.period_);
}

template <typename T>
float randMinMax(T min_val, T max_val)
{
    // std::cout << max_val - min_val << std::endl;
    return min_val + static_cast<T>(std::rand()) / (static_cast<T>(RAND_MAX / (max_val - min_val + 1)));
}

std::vector<float> UUnifast(int ntasks, float utilization)
{
    float sum_u = utilization;
    float next_sum_u;
    std::vector<float> utils;
    for (int i = 0; i < ntasks - 1; i++)
    {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        next_sum_u = sum_u * std::pow(r, 1.0 / static_cast<float>(ntasks - i));
        utils.push_back(sum_u - next_sum_u);
        sum_u = next_sum_u;
    }
    utils.push_back(sum_u);
    return utils;
}

std::vector<std::pair<int, int>> WatersChallenge2015::genIndexPerPeriods(std::vector<Task> &taskset)
{
    std::vector<std::pair<int, int>> indexes_per_periods(N_PERIODS, std::pair(-1, -1));
    std::sort(taskset.begin(), taskset.end(), compareTasksPeriod);

    for (auto t : taskset)
        t.print();

    int start = 0, end = -1, i;
    std::size_t j;
    for (i = 0, j = 0; i < N_PERIODS && j < taskset.size(); j++)
    {
        if (periods_[i] != taskset[j].period_)
        {
            indexes_per_periods[i].first = start;
            indexes_per_periods[i].second = end;

            while (periods_[i] != taskset[j].period_)
                i++;
            start = j;
        }
        end++;
    }
    if (i < N_PERIODS)
    {
        indexes_per_periods[i].first = start;
        indexes_per_periods[i].second = end;
    }
    return indexes_per_periods;
}

int WatersChallenge2015::getRandomTaskIndexGivenPeriodIndex(std::vector<std::pair<int, int>> ipp, int p_index)
{
    int task_index = -1;
    if (ipp[p_index].second - ipp[p_index].first > 0)
        task_index = randMinMax<int>(ipp[p_index].first, ipp[p_index].second);
    else if (ipp[p_index].second - ipp[p_index].first == 0 && ipp[p_index].second >= 0)
        task_index = ipp[p_index].second;
    return task_index;
}
WatersChallenge2015::WatersChallenge2015(const int n_tasks, const int n_chains, const float utilization, task_gen_t generation, DataFiles &f, const int n_cores)
{

    std::cout << "Period\tShare\tACET\tFminB\tFmaxB\tFminW\tFmaxW" << std::endl;
    for (std::size_t i = 0; i < periods_.size(); i++)
        std::cout << periods_[i] << "\t" << p_shares_[i] << "\t" << acet_[i] << "\t" << fmin_best_[i] << "\t" << fmax_best_[i] << "\t" << fmin_worst_[i] << "\t" << fmax_worst_[i] << "\t" << std::endl;

    generator_.seed(time(NULL));
    p_distribution_ = new std::discrete_distribution<int>(p_shares_.begin(), p_shares_.end());
    spic_distribution_ = new std::discrete_distribution<int>(spic_shares_.begin(), spic_shares_.end());
    dpic_distribution_ = new std::discrete_distribution<int>(dpic_shares_.begin(), dpic_shares_.end());
    for (auto cs : comunication_shares_)
    {
        std::discrete_distribution<int> distribution(cs.begin(), cs.end());
        comunication_distribution_.push_back(distribution);
    }

    n_cores_ = n_cores;
    // std::vector<Task> taskset;

    switch (generation)
    {
    case ONLY_N:
        taskset_ = generateTasksetGivenN(n_tasks);
        break;
    case ONLY_U:
        taskset_ = generateTasksetGivenU(utilization);
        break;
    case N_AND_U:
        taskset_ = generateTasksetUUnifast(n_tasks, utilization);
        break;
    case TEST_1:
        Task t1, t2, t3;
        t1.period_ = 4;
        t1.wcet_ = 2;
        t1.bcet_ = 2;
        taskset_.push_back(t1);

        t2.period_ = 8;
        t2.wcet_ = 1;
        t2.bcet_ = 1;
        taskset_.push_back(t2);

        t3.period_ = 2;
        t3.wcet_ = 1;
        t3.bcet_ = 1;
        taskset_.push_back(t3);

        period_mult_ = 1;
        break;
    case TEST_2:
        Task t4;
        t1.period_ = 100;
        t1.wcet_ = 86;
        t1.bcet_ = 86;
        taskset_.push_back(t1);

        t2.period_ = 100;
        t2.wcet_ = 31;
        t2.bcet_ = 31;
        taskset_.push_back(t2);

        t3.period_ = 200;
        t3.wcet_ = 79;
        t3.bcet_ = 79;
        taskset_.push_back(t3);

        t4.period_ = 200;
        t4.wcet_ = 38;
        t4.bcet_ = 38;
        taskset_.push_back(t4);

        period_mult_ = 1;
        break;
    };

    // std::vector<std::vector<int>> chains;
    if (generation == TEST_1)
    {
        chains_.push_back({0, 1, 2});
    }
    else if (generation == TEST_2)
    {
        chains_.push_back({3, 2, 1, 0});
    }
    else
    {
        std::sort(taskset_.begin(), taskset_.end(), compareTasksPeriod);
        chains_ = generateChains(taskset_, n_chains);
    }

    edges_ = Eigen::MatrixXf::Zero(n_tasks, n_tasks);

    for (auto c : chains_)
    {
        for (size_t i = 0; i < c.size() - 1; i++)
        {
            edges_(c[i], c[i + 1])++;
            edges_(c[i + 1], c[i])++;
        }
    }
}

bool WatersChallenge2015::SoAcomparison(DataFiles &f)
{
    MultiRateTaskset mtaskset;
    VariableTaskSet vtaskset;
    std::vector<std::shared_ptr<MultiNode>> mtasks;
    std::vector<std::shared_ptr<MultiNode>> vtasks;
    for (auto task : taskset_)
    {
        auto mtask_ref = mtaskset.addTask(task.period_ * period_mult_, task.wcet_);
        mtask_ref->bcet = task.bcet_;
        mtasks.push_back(mtask_ref);

        auto vtask_ref = vtaskset.addTask(task.period_ * period_mult_, task.wcet_);
        vtask_ref->bcet = task.bcet_;
        vtasks.push_back(vtask_ref);
    }

    for (int i = 0; i < edges_.rows(); i++)
    {
        for (int j = i + 1; j < edges_.cols(); j++)
        {
            if (edges_(i, j) > 0 && mtasks[i]->period != mtasks[j]->period)
            {
                mtaskset.addDataEdge(mtasks[i], mtasks[j], 0);
                vtaskset.addDataEdge(vtasks[i], vtasks[j]);
            }
        }
    }

    if (vtaskset.computePermutations() > 300)
        return false;

    time_t tstart, tend, tend_Forget, tend_Saidi;
    tstart = time(0);

    mtaskset.createBaselineDAG();

    Evaluation eval_other;
    std::vector<std::shared_ptr<MultiNode>> lotasks;
    for (auto c : chains_)
    {
        lotasks.clear();
        for (auto t : c)
            lotasks.push_back(mtasks[t]);
        eval_other.addLatency(lotasks, LatencyCost(50, 1), LatencyConstraint());
    }
    eval_other.addScheduling(SchedulingCost(10), SchedulingConstraint(n_cores_));

    tend = time(0);

    //FORGET -----------------------------------------------------------------------------------------------
    std::cout << " *************************FORGET *************************" << std::endl;
    auto dags_Forget = mtaskset.createDAGs(&f);
    if (dags_Forget.size() > 0)
        eval_other.evaluate(dags_Forget, &f);
    else
    {
        f.sd << "0;";
        f.d << "0;";
        f.p << "0;";
    }

    tend_Forget = time(0);
    std::cout << "It took " << difftime(tend_Forget, tstart) << " second(s)." << std::endl;
    f.t << difftime(tend_Forget, tstart) << ";";

    //SAIDI -----------------------------------------------------------------------------------------------
    f.addTest();
    std::cout << " *************************SAIDI *************************" << std::endl;
    auto dags_Saidi = mtaskset.createDAGs(&f, true);
    if (dags_Saidi.size() > 0)
        eval_other.evaluate(dags_Saidi, &f);
    else
    {
        f.sd << "0;";
        f.d << "0;";
        f.p << "0;";
    }

    tend_Saidi = time(0);
    std::cout << "It took " << difftime(tend, tstart) + difftime(tend_Saidi, tend_Forget) << " second(s)." << std::endl;
    f.t << difftime(tend, tstart) + difftime(tend_Saidi, tend_Forget) << ";";

    //VERUCCHI -----------------------------------------------------------------------------------------------
    f.addTest();
    std::cout << " *************************VERUCCHI *************************" << std::endl;
    tstart = time(0);

    vtaskset.createBaselineTaskset();
    auto &dags_Verucchi = vtaskset.createDAGs(&f);

    Evaluation eval;
    std::vector<std::shared_ptr<MultiNode>> ltasks;
    for (size_t i = 0; i < chains_.size(); i++)
    {
        ltasks.clear();
        for (auto t : chains_[i])
            ltasks.push_back(mtasks[t]);
        eval.addLatency(ltasks, LatencyCost(50, 1), LatencyConstraint());
    }
    eval.addScheduling(SchedulingCost(10), SchedulingConstraint(n_cores_));

    if (dags_Verucchi.size() > 0)
        eval.evaluate(dags_Verucchi, &f);
    else
    {
        f.sd << "0;";
        f.d << "0;";
        f.p << "0;";
    }
    tend = time(0);
    std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;
    f.t << difftime(tend, tstart) << ";";

    //BECKER-- ---------------------------------------------------------------------------------------------
    f.addTest();
    std::cout << " *************************BECKER *************************" << std::endl;
    tstart = time(0);
    if (dags_Verucchi.size() > 0)
    {
        float m_latency, deadline, min_max_latency, new_lat, deadline_up, deadline_down;
        int chain_i = 0;
        for (auto c : chains_)
        {
            for (auto tc : c)
                std::cout << " ->" << tc;
            std::cout << std::endl;
            Becker b(mtasks);
            m_latency = b.computeChainMinMaxLatency(c);

            deadline = m_latency;
            min_max_latency = deadline;
            new_lat = deadline;

            deadline_up = deadline;
            deadline_down = 0;

            deadline = std::ceil(deadline / 2.0);

            while (deadline_up - deadline_down > 1)
            {
                Becker b(mtasks);
                new_lat = b.synthesizeDependencies(c, deadline);
                if (new_lat > 0)
                {
                    min_max_latency = new_lat;
                    deadline_up = deadline;
                }
                else
                {
                    deadline_down = deadline;
                }
                deadline = deadline_down + std::ceil((deadline_up - deadline_down) / 2.0);
                std::cout << "deadline:" << m_latency << "up:" << deadline_up << "down:" << deadline_down << std::endl;
            }

            // new_lat = b.synthesizeDependencies(c, deadline_up, true);
            std::cout << "max latency:" << m_latency << std::endl;

            if (f.da_values[f.val_index - 1].size() > 0 && f.da_values[f.val_index - 1][chain_i] > min_max_latency && min_max_latency > 0)
            {
                min_max_latency = f.da_values[f.val_index - 1][chain_i];
            }

            std::cout << "min max latency:" << min_max_latency << std::endl;

            f.addDA(min_max_latency);
            chain_i++;
        }
    }
    tend = time(0);
    std::cout << "It took " << difftime(tend, tstart) << " second(s)." << std::endl;
    f.t << difftime(tend, tstart) << ";";

    f.newLine();
    f.writeRDA(chains_.size());
    return true;
}

std::vector<std::vector<int>> WatersChallenge2015::generateChains(std::vector<Task> taskset, int n_chains)
{

    auto indexes_per_periods = genIndexPerPeriods(taskset);
    std::vector<int> n_task_per_period;
    int actual_different_periods = 0;
    for (auto ipp : indexes_per_periods)
    {
        n_task_per_period.push_back(ipp.second < 0 ? 0 : ipp.second - ipp.first + 1);
        actual_different_periods += (ipp.second < 0 ? 0 : 1);
    }

    for (int i = 0; i < N_PERIODS; i++)
        std::cout << "[" << indexes_per_periods[i].first << ", " << indexes_per_periods[i].second << "]  \tRandom: " << getRandomTaskIndexGivenPeriodIndex(indexes_per_periods, i) << std::endl;

    int n_dif_periods, cur_task, start_freq, cur_period;
    std::vector<bool> used_task(taskset.size(), false);
    std::vector<int> chain;
    std::vector<std::vector<int>> chains;
    for (int i = 0; i < n_chains; i++)
    {
        //reset
        chain.clear();
        std::fill(used_task.begin(), used_task.end(), 0);

        //number of different period in a chain
        n_dif_periods = std::min(different_periods_in_chain_[(*dpic_distribution_)(generator_)], actual_different_periods);

        //first period frequence
        start_freq = std::min(same_period_in_chains_[(*spic_distribution_)(generator_)], *std::max_element(n_task_per_period.begin(), n_task_per_period.end()));
        std::cout << "number of diff periods in the chain: " << n_dif_periods << std::endl;

        //first period in the chain
        cur_period = (*p_distribution_)(generator_);
        while (n_task_per_period[cur_period] < start_freq)
            cur_period = (*p_distribution_)(generator_);

        if (n_dif_periods == 1)
        {
            //first task in the chain
            cur_task = getRandomTaskIndexGivenPeriodIndex(indexes_per_periods, cur_period);
            chain.push_back(cur_task);
            used_task[cur_task] = true;
            //chain with only one period
            for (int i = 0; i < start_freq - 1;)
            {
                cur_task = getRandomTaskIndexGivenPeriodIndex(indexes_per_periods, cur_period);

                if (!used_task[cur_task])
                {
                    chain.push_back(cur_task);
                    used_task[cur_task] = true;
                    i++;
                }
            }
        }
        else
        {
            //chain with more periods
            std::vector<int> periods_selected(n_dif_periods, -1);
            std::vector<int> periods_frequences(N_PERIODS, 0);
            std::vector<int> periods_used(N_PERIODS, 0);

            periods_selected[0] = cur_period;
            periods_frequences[cur_period] = start_freq;

            int chain_length = start_freq;

            for (std::size_t i = 1; i < periods_selected.size(); i++)
            {
                while (std::find(periods_selected.begin(), periods_selected.end(), cur_period) != periods_selected.end() || n_task_per_period[cur_period] <= 0)
                {
                    cur_period = comunication_distribution_[periods_selected[i - 1]](generator_);
                }
                periods_selected[i] = cur_period;
                // std::cout << cur_period << " " << n_task_per_period[cur_period] << std::endl;
                periods_frequences[cur_period] = std::min(same_period_in_chains_[(*spic_distribution_)(generator_)], n_task_per_period[cur_period]);
                chain_length += periods_frequences[cur_period];
            }

            // std::cout << "periods selected: " << std::endl;
            // for (std::size_t i = 0; i < periods_selected.size(); i++)
            //     std::cout << periods_selected[i] << " " << n_task_per_period[periods_selected[i]] << " " << periods_frequences[periods_selected[i]] << std::endl;

            for (int i = 0, next_period; i < chain_length; i++)
            {
                if (i == 0)
                {
                    //first task of the chain
                    next_period = periods_selected[randMinMax<int>(0, periods_selected.size() - 1)];
                }
                else
                {
                    next_period = comunication_distribution_[cur_period](generator_);
                    while (std::find(periods_selected.begin(), periods_selected.end(), next_period) == periods_selected.end() || periods_used[next_period] >= periods_frequences[next_period])
                        next_period = comunication_distribution_[cur_period](generator_);
                }

                periods_used[next_period]++;

                cur_task = getRandomTaskIndexGivenPeriodIndex(indexes_per_periods, next_period);
                while (used_task[cur_task])
                    cur_task = getRandomTaskIndexGivenPeriodIndex(indexes_per_periods, next_period);

                chain.push_back(cur_task);
                used_task[cur_task] = true;

                // std::cout << "period selected: " << next_period << std::endl;
                // std::cout << "task selected: " << cur_task << std::endl;
                cur_period = next_period;
            }
        }

        for (auto c : chain)
            std::cout << c << ":" << taskset[c].period_ << "\t";
        std::cout << std::endl;

        //add chain
        chains.push_back(chain);
    }
    return chains;
}

std::vector<Task> WatersChallenge2015::generateTasksetGivenU(float utilization)
{
    std::cout << "Task generation" << std::endl;
    std::vector<Task> tasks;
    float u_sum = 0;
    while (u_sum < utilization - 0.05)
    {
        tasks.push_back(generateTask());
        u_sum += tasks.back().utilization_;
    }
    std::cout << "Final utilization: " << u_sum << "\tN tasks: " << tasks.size() << std::endl;

    return tasks;
}

std::vector<Task> WatersChallenge2015::generateTasksetGivenN(int max_n)
{
    std::cout << "Task generation" << std::endl;
    std::vector<Task> tasks;
    float u_sum = 0;
    int n_tasks = 0;
    while (n_tasks < max_n)
    {
        tasks.push_back(generateTask());
        u_sum += tasks.back().utilization_;
        n_tasks++;
    }
    std::cout << "Final utilization: " << u_sum << "\tN tasks: " << n_tasks << std::endl;
    return tasks;
}

std::vector<Task> WatersChallenge2015::generateTasksetUUnifast(int max_n, float utilization)
{
    std::cout << "Task generation" << std::endl;
    std::vector<Task> tasks;
    float u_sum = 0;
    auto utils = UUnifast(max_n, utilization);
    for (auto u : utils)
    {
        tasks.push_back(generateTask(u));
        u_sum += tasks.back().utilization_;
    }

    std::cout << "Final utilization: " << u_sum << "\tN tasks: " << tasks.size() << std::endl;
    return tasks;
}

Task WatersChallenge2015::generateTask(float util)
{
    Task t;
    int index = (*p_distribution_)(generator_);
    t.period_ = periods_[index];

    if (util == -1)
    {
        t.wcet_ = acet_[index] * randMinMax<float>(fmin_worst_[index], fmax_worst_[index]);
        t.bcet_ = acet_[index] * randMinMax<float>(fmin_best_[index], fmax_best_[index]);
    }
    else
    {
        t.wcet_ = (t.period_ * 1000) * util;
        float avg = t.wcet_ / randMinMax<float>(fmin_worst_[index], fmax_worst_[index]);
        t.bcet_ = avg * randMinMax<float>(fmin_best_[index], fmax_best_[index]);
    }

    t.utilization_ = t.wcet_ / (t.period_ * 1000);

    t.print();
    return t;
}

WatersChallenge2015::~WatersChallenge2015()
{
    delete p_distribution_;
    delete spic_distribution_;
    delete dpic_distribution_;
}