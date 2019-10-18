#include "Becker.h"

void Becker::printIntervals()
{
    std::cout << "Rmin\tRmax\tDmin\tDmax\tDminL" << std::endl;
    for (size_t i = 0; i < Dmin.size(); i++)
    {
        for (size_t j = 0; j < Dmin[i].size(); j++)
        {
            std::cout << Rmin[i][j] << "\t" << Rmax[i][j] << "\t" << Dmin[i][j] << "\t" << Dmax[i][j] << "\t" << Dmin_local[i][j] << std::endl;
        }
        std::cout << "---------------------" << std::endl;
    }
}

Becker::Becker(std::vector<std::shared_ptr<MultiNode>> mtasks)
{
    n_tasks_ = mtasks.size();
    mtasks_ = mtasks;

    // for (auto task : mtasks_)
    //     std::cout << task->period << std::endl;

    Rmin.resize(mtasks_.size());
    Rmax.resize(mtasks_.size());
    Dmin.resize(mtasks_.size());
    Dmin_local.resize(mtasks_.size());
    Dmax.resize(mtasks_.size());
}

void Becker::computeIntervals(int hyperperiod_count)
{
    for (int i = 0; i < n_tasks_; i++)
    {
        int n_jobs = hyperperiod_ * hyperperiod_count / mtasks_[i]->period;
        Rmin[i].resize(n_jobs);
        Rmax[i].resize(n_jobs);
        Dmin[i].resize(n_jobs);
        Dmin_local[i].resize(n_jobs);
        Dmax[i].resize(n_jobs);
        for (size_t j = 0; j < n_jobs; j++)
        {
            Rmin[i][j] = j * mtasks_[i]->period;
            Rmax[i][j] = (j + 1) * mtasks_[i]->period - mtasks_[i]->wcet;
            Dmin[i][j] = Rmin[i][j] + mtasks_[i]->wcet;
            Dmin_local[i][j] = Dmin[i][j];
            Dmax[i][j] = (j + 2) * mtasks_[i]->period;

            // std::cout << Rmin[i][j] << " " << Rmax[i][j] << " " << Dmin[i][j] << " " << Dmax[i][j] << std::endl;
        }
        // std::cout << "---------------------" << std::endl;
    }
}

void Becker::initilizeDminLoc()
{
    for (size_t i = 0; i < Dmin.size(); i++)
        for (size_t j = 0; j < Dmin[i].size(); j++)
            Dmin_local[i][j] = Dmin[i][j];
}

void Becker::buildPropTree(const std::vector<int> &chain, std::pair<int, int> &succ_root, std::vector<std::pair<int, int>> &leaves, const int i, const int j, int i_chain, DPT &tree)
{

    // std::cout << i << " " << j << " *******" << std::endl;
    i_chain++;
    if (i_chain < chain.size())
    {

        int k = chain[i_chain];

        int l = computeL(i, j, k);

        for (size_t z = l; z < Dmax[k].size(); z++)
        {

            // std::cout << "before follows: " << k << " " << z << " +++++++" << std::endl;

            if (follows(i, j, k, z))
            {

                // std::cout << Rmax[k][z] << ">= " << Dmin_local[i][j] << " and" << Rmin[k][z] << "< " << Dmax[i][j] << std::endl;

                tree.insertChild(i, j, k, z, tree.root);
                //update Dmin
                Dmin_local[k][z] = std::max(Dmin[i][j] + mtasks_[k]->wcet, Dmin[k][z]);
                buildPropTree(chain, succ_root, leaves, k, z, i_chain, tree);
                if (i_chain + 1 == chain.size())
                    leaves.push_back(std::pair<int, int>(k, z));
                if (i_chain - 1 == 0 && z == l)
                {
                    succ_root.first = k;
                    succ_root.second = z;
                }
            }
        }
    }
}

float Becker::synthesizeDependencies(const std::vector<int> &chain, float deadline, bool verbose)
{

    float max_latency = 0;
    if (chain.size() > 0)
    {

        // std::cout << "hyperperiod " << hyperperiod_ << std::endl;
        hyperperiod_ = mtasks_[chain[0]]->period;
        for (int i = 1; i < chain.size(); i++)
            hyperperiod_ = std::lcm(hyperperiod_, mtasks_[chain[i]]->period);
        // for (int i = 1; i < mtasks_.size(); i++)
        //     hyperperiod_ = std::lcm(hyperperiod_, mtasks_[i]->period);

        // std::cout << "hyperperiod " << hyperperiod_ << std::endl;

        // for (auto c : chain)
        //     std::cout << c << "\t";
        // std::cout << std::endl;
        int WCL = 0;
        for (auto c : chain)
            WCL += 2 * mtasks_[c]->period;

        // std::cout << "WCL:" << WCL << std::endl;
        int hyperperiod_count = std::ceil(WCL / static_cast<float>(hyperperiod_));
        // std::cout << "hc:" << hyperperiod_count << std::endl;

        computeIntervals(hyperperiod_count);
        // printIntervals();

        float cur_max_latency;
        int i = chain[0];
        bool dep = true;
        float max_lat_thresh = WCL;
        int first_invalid_k = -1;
        int first_invalid_l = -1;
        while (max_lat_thresh > deadline && dep)
        {
            allDPT at;
            if (verbose)
            {
                std::cout << "build trees" << std::endl;
                printIntervals();
            }
            max_latency = 0;
            for (int j = 0; j < Dmax[i].size(); j++)
            {
                std::vector<std::pair<int, int>> leaves;
                std::pair<int, int> succ_root;
                DPT tree(i, j);
                initilizeDminLoc();
                buildPropTree(chain, succ_root, leaves, i, j, 0, tree);
                at.trees.push_back(tree);
                cur_max_latency = tree.calcMaxLatency(Rmax, Rmin, mtasks_, chain[chain.size() - 1]);
                if (cur_max_latency > max_latency)
                    max_latency = cur_max_latency;
            }
            max_lat_thresh = max_latency;
            if (verbose)
                at.printTrees(Rmax, Rmin, mtasks_);

            if (max_latency > deadline)
            {
                if (verbose)
                    std::cout << "deadline:" << deadline << " cur_max_lat: " << cur_max_latency << std::endl;
                int k = -1, l = -1, z = -1, w = -1;

                if (first_invalid_k == -1 && first_invalid_l == -1 && k == -1 && l == -1)
                {
                    for (int j = 0; j < Dmax[i].size(); j++)
                    {
                        at.trees[j].findFirstInvalid(at.trees[j].root, Rmax, Rmin, mtasks_, deadline, k, l);
                        if (k != -1 && l != -1)
                        {
                            first_invalid_k = k;
                            first_invalid_l = l;
                            break;
                        }
                    }
                }
                else
                {
                    k = first_invalid_k;
                    l = first_invalid_l;
                }

                if (verbose)
                    std::cout << "First invalid: " << k << " " << l << std::endl;

                bool res = false;
                while (!res)
                {
                    if (k != -1 && l != -1)
                    {
                        // std::cout << "node  : " << k << " " << l << std::endl;
                        // at.printTrees();
                        at.findDad(k, l, z, w);
                        if (z != -1 && w != -1)
                        {
                            // std::cout << "dad  : " << z << " " << w << std::endl;
                            if (w + 1 < Dmin[z].size())
                            {
                                // std::cout << "dad sibling : " << z << " " << ++w << std::endl;

                                int z_k_hyp = std::lcm(mtasks_[z]->period, mtasks_[k]->period);

                                int z_instances = z_k_hyp / mtasks_[z]->period;
                                int k_instances = z_k_hyp / mtasks_[k]->period;

                                for (int hc = 0; hc < hyperperiod_count * hyperperiod_ / z_k_hyp; hc++)
                                    res = insertJobLevelDependency(z, w % z_instances + hc * z_instances, k, l % k_instances + hc * k_instances);

                                // int z_instances = hyperperiod_ / mtasks_[z]->period;
                                // int k_instances = hyperperiod_ / mtasks_[k]->period;

                                // for (int hc = 0; hc < hyperperiod_count; hc++)
                                //     res = insertJobLevelDependency(z, w % z_instances + hc * z_instances, k, l % k_instances + hc * k_instances);
                                k = z;
                                l = w;
                                z = -1;
                                w = -1;
                            }
                            if (verbose)
                            {
                                for (auto d : dependencies)
                                    std::cout << d.tf_ << " " << d.jf_ << " " << d.tt_ << " " << d.jt_ << std::endl;
                            }
                        }
                        else
                            break;
                    }
                    else
                        break;
                }

                dep = res;
            }
        }
        // std::cout << "maxLatency: " << max_latency << std::endl;

        if (max_latency > deadline)
            return 0;
    }
    return max_latency;
}

float Becker::computeChainMinMaxLatency(const std::vector<int> &chain)
{

    float min_latency = std::numeric_limits<float>::max();
    float max_latency = 0;
    if (chain.size() > 0)
    {
        hyperperiod_ = mtasks_[chain[0]]->period;
        for (int i = 1; i < chain.size(); i++)
            hyperperiod_ = std::lcm(hyperperiod_, mtasks_[chain[i]]->period);
        // for (auto c : chain)
        //     std::cout << c << "\t";
        // std::cout << std::endl;
        int WCL = 0;
        for (auto c : chain)
            WCL += 2 * mtasks_[c]->period;

        // std::cout << "WCL:" << WCL << std::endl;
        int hyperperiod_count = std::ceil(WCL / static_cast<float>(hyperperiod_));
        // std::cout << "hc:" << hyperperiod_count << std::endl;

        computeIntervals(hyperperiod_count);

        float cur_min_latency, cur_max_latency;
        int i = chain[0];
        for (int j = 0; j < Dmax[i].size(); j++)
        {

            std::vector<std::pair<int, int>> leaves;
            std::pair<int, int> succ_root;
            DPT tree(i, j);

            buildPropTree(chain, succ_root, leaves, i, j, 0, tree);

            // tree.calcMaxLatency(Rmax, Rmin, mtasks_);
            // tree.printTree(*tree.root);

            for (auto l : leaves)
            {
                cur_min_latency = minLatency(i, j, l.first, l.second, succ_root.first, succ_root.second);
                cur_max_latency = maxLatency(i, j, l.first, l.second);
                // printIntervals();

                // std::cout << i << " " << j << " " << l.first << " " << l.second << " " << succ_root.first << " " << succ_root.second << " min: " << cur_min_latency << " max: " << cur_max_latency << std::endl;

                if (cur_max_latency > max_latency)
                    max_latency = cur_max_latency;
                if (cur_min_latency < min_latency && cur_min_latency > 0)
                    min_latency = cur_min_latency;
            }

            // tree.printTree(tree.root, Rmax, Rmin, mtasks_);
            // std::cout << "###################################" << std::endl;
        }
        std::cout << "minLatency: " << min_latency << "\tmaxLatency: " << max_latency << std::endl;
    }
    return max_latency;
}

float Becker::maxLatency(int i, int j, int k, int l)
{
    return (Rmax[k][l] + mtasks_[k]->wcet) - Rmin[i][j];
}
float Becker::minLatency(int i, int j, int k, int l, int z, int w)
{
    return Dmin[k][l] - (std::max(Rmin[i][j], Rmin[z][w]) - mtasks_[i]->wcet);
}

int Becker::computeL(int i, int j, int k)
{
    return std::floor(Dmin_local[i][j] / mtasks_[k]->period);
    // return std::floor(Dmin[i][j] / mtasks_[k]->period);
}

bool Becker::follows(int i, int j, int k, int l)
{
    if (Rmax[k][l] - Rmin[k][l] >= 0)
    {
        // std::cout << "\tfollows:" << Rmax[k][l] << ">=" << Dmin_local[i][j] << " and " << Rmin[k][l] << " < " << Dmax[i][j] << std::endl;

        if (Rmax[k][l] >= Dmin_local[i][j] && Rmin[k][l] < Dmax[i][j] && logicalBoundariesCheck(i, j, k, l))
            return true;
    }
    return false;
}

bool Becker::logicalBoundariesCheck(int i, int j, int k, int l)
{
    for (int a = 0; a < dependencies.size(); a++)
    {
        if (dependencies[a].tf_ == i && dependencies[a].tt_ == k)
        {
            if (dependencies[a].jf_ == j)
            {
                int k_instances = hyperperiod_ / mtasks_[k]->period;
                if (l < dependencies[a].jt_ + k_instances)
                    return true;
                else
                    return false;
            }
        }
    }
    return true;
}

bool Becker::insertJobLevelDependency(int i, int j, int k, int l)
{
    for (auto d : dependencies)
        if (d.tf_ == i && d.jf_ == j && d.tt_ == k && d.jt_ == l)
            return false;

    JLD jld(i, j, k, l);
    dependencies.push_back(jld);

    float max_Rmin = 0;

    for (int a = 0; a < dependencies.size(); a++)
    {
        if (dependencies[a].tt_ == k && dependencies[a].jt_ == l) //all the predecessors of k,l
        {
            if (Rmin[dependencies[a].tf_][dependencies[a].jf_] + mtasks_[dependencies[a].tf_]->wcet > max_Rmin)
                max_Rmin = Rmin[dependencies[a].tf_][dependencies[a].jf_] + mtasks_[dependencies[a].tf_]->wcet;
        }
    }

    float min_Rmax = std::numeric_limits<float>::max();
    for (int a = 0; a < dependencies.size(); a++)
    {
        if (dependencies[a].tf_ == i && dependencies[a].jf_ == j) //all the successor of i,j
        {
            if (Rmax[dependencies[a].tt_][dependencies[a].jt_] - mtasks_[dependencies[a].tf_]->wcet < min_Rmax)
                min_Rmax = Rmax[dependencies[a].tt_][dependencies[a].jt_] - mtasks_[dependencies[a].tf_]->wcet;
        }
    }

    float newRminkl = std::max(max_Rmin, Rmin[k][l]);
    float newRmaxij = std::min(min_Rmax, Rmax[i][j]);
    float newDmaxij;
    if (j > 0)
        newDmaxij = newRmaxij + mtasks_[i]->wcet;

    if (newRminkl > Rmax[k][l] || Rmin[i][j] > newRmaxij || (j > 0 && Dmin[i][j] > newDmaxij))
    {
        // std::cout << "YUCK!!!!!!!!!!!!!!!!" << std::endl;
        return false;
    }

    //values update
    Rmin[k][l] = newRminkl;
    Rmax[i][j] = newRmaxij;
    if (j > 0)
        Dmax[i][j - 1] = newDmaxij;

    return true;
}

Becker::~Becker()
{

    mtasks_.clear();
    for (auto r : Rmin)
        r.clear();
    Rmin.clear();

    for (auto r : Rmax)
        r.clear();
    Rmax.clear();

    for (auto d : Dmin)
        d.clear();
    Dmin.clear();

    for (auto d : Dmin_local)
        d.clear();
    Dmin_local.clear();

    for (auto d : Dmax)
        d.clear();
    Dmax.clear();
    dependencies.clear();
}
