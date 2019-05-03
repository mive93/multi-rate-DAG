#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "DAG/DAG.h"
#include "DAG/Node.h"
#include <vector>
#include <iostream>

struct ScheduleInfos
{
    std::shared_ptr<Node> n;
    double est;
    double lst;
    double eft;
    double lft;
};

bool compareSchedulingInfos(ScheduleInfos &a, ScheduleInfos &b)
{
    return ((a.est > b.est) || (a.est == b.est && a.lft > b.lft));
}

void printSchedule(const std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule)
{
    for (size_t i = 0; i < processors_schedule.size(); i++)
    {
        std::cout << "---Processor: " << i << "\t";
        for (auto n : processors_schedule[i])
            std::cout << n->uniqueId << "(" << n->wcet << "); ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void scheduleToTikz(const std::string filename, const std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule, const double period)
{
    //opening the tex file
    std::ofstream tikz_file;
    tikz_file.open(filename);

    //beginning the tikz figure
    tikz_file << "\\documentclass[tikz,border=10pt]{standalone}\n"
                 "\\usepackage{tkz-graph}\n"
                 "\\usetikzlibrary{automata}\n"
                 "\\usetikzlibrary[automata]\n"
                 "\\begin{document}\n";

    tikz_file << "\\begin{tikzpicture}\n";

    double x = 0, y = 0;
    int proc = 1;
    for (auto p : processors_schedule)
    {
        tikz_file << "\\draw (0," << y << ") --(0," << y + 1 << ") ;" << std::endl;
        tikz_file << "\\draw[<-] (" << period + 0.2 << "," << y << ") ->(" << period + 0.2 << "," << y + 1 << ") ;" << std::endl;
        tikz_file << "\\draw (-0.2," << y << ") --(" << period + 0.5 << "," << y << ") ;" << std::endl;
        tikz_file << "\\draw (0-0.5," << y + 0.5 << ") node {$P_{" << proc << "}$};" << std::endl;

        x = 0;
        for (auto n : p)
        {
            if (n->groupId != 888)
                tikz_file << "\\draw (" << x << "," << y << ") rectangle (" << x + n->wcet << "," << y + 0.7 << ") node[pos=.5] {$" << n->shortName << "$};" << std::endl;
            tikz_file << "\\draw (" << x << "," << y << ") node[below] {$" << x << "$};" << std::endl;
            x += n->wcet;
        }

        y += 2;
        proc++;
    }

    //ending tikz figure
    tikz_file << "\\end{tikzpicture}\n";
    tikz_file << "\\end{document}\n";

    //close the file
    tikz_file.close();
}

std::vector<ScheduleInfos> createScheduleInfos(const DAG &dag, unsigned &last_uniqueId, bool verbose = false)
{
    std::vector<ScheduleInfos> nodes_to_sched;
    auto nodes = dag.getNodes();
    auto infos = dag.getNodeInfo();

    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->groupId != 0 && nodes[i]->groupId != 667 && nodes[i]->groupId != 666)
        {
            ScheduleInfos si;
            si.n = nodes[i];
            si.est = infos.est[i];
            si.lst = infos.lst[i];
            si.eft = infos.eft[i];
            si.lft = infos.lft[i];
            nodes_to_sched.push_back(si);
        }
        if (i == nodes.size() - 1)
            last_uniqueId = nodes[i]->uniqueId;
    }

    std::sort(nodes_to_sched.begin(), nodes_to_sched.end(), compareSchedulingInfos);

    if (verbose)
    {
        std::cout << "ID\tWCET\tBCET\tEST\tLST\tEFT\tEFT" << std::endl;
        for (auto n : nodes_to_sched)
            std::cout << n.n->uniqueId << "\t" << n.n->wcet << "\t" << n.n->bcet << "\t" << n.est << "\t" << n.lst << "\t" << n.eft << "\t" << n.eft << std::endl;
    }

    return nodes_to_sched;
}

bool scheduleDAG(const DAG &dag, const unsigned n_proc, const bool verbose = false, const std::string filename = "")
{

    unsigned last_uniqueId;

    std::vector<ScheduleInfos> nodes_to_sched = createScheduleInfos(dag, last_uniqueId);
    std::vector<ScheduleInfos> ready;

    std::vector<std::vector<std::shared_ptr<Node>>> processors_schedule(n_proc);
    std::vector<double> processor_usage(n_proc, 0);

    const double max_delta = 1000;
    double epsilon = 0.0000001;
    double delta = max_delta;

    for (double t = 0; t < dag.getPeriod() && !(ready.empty() && nodes_to_sched.empty()); t += delta)
    {

        //create ready queue
        while (!nodes_to_sched.empty() && nodes_to_sched.back().est <= t + epsilon)
        {
            ready.push_back(nodes_to_sched.back());
            nodes_to_sched.pop_back();
        }
        std::sort(ready.begin(), ready.end(), compareSchedulingInfos);

        //update processor state
        for (size_t i = 0; i < n_proc; i++)
        {
            if (processor_usage[i] > 0)
            {
                processor_usage[i] -= delta;
                if (processor_usage[i] < epsilon)
                    processor_usage[i] = 0;
            }
        }

        // compute delta: next point in time to check
        delta = max_delta;
        auto max_ut = *(std::max_element(std::begin(processor_usage), std::end(processor_usage)));
        if (max_ut > 0 + epsilon)
            delta = max_ut;
        for (auto r : ready)
            if (r.n->wcet < delta)
                delta = r.n->wcet;
        for (auto p : processor_usage)
            if (p > 0 && p < delta)
                delta = p;
        if (!nodes_to_sched.empty())
            delta = (delta < nodes_to_sched.back().est - t) ? delta : nodes_to_sched.back().est - t;

        if (delta < epsilon)
        {
            std::cout << "Something is VERY WRONG" << std::endl;
            return false;
        }

        //delta = 0.1;

        //if a processor is avaiable and there are ready tasks, schedule them, otherwise schedule an idle task
        for (size_t i = 0; i < n_proc; i++)
        {
            if (std::abs(processor_usage[i] - 0) <= epsilon)
            {
                if (!ready.empty())
                {
                    auto task_chosen = ready.back();
                    ready.pop_back();
                    if (t + task_chosen.n->wcet > task_chosen.lft)
                    {
                        if (verbose)
                        {
                            std::cout << "Failed at t = " << t << "; ";
                            std::cout << "task actual ft:" << t + task_chosen.n->wcet << " task lft:" << task_chosen.lft << "; ";
                            std::cout << task_chosen.n->uniqueId << "(" << task_chosen.n->wcet << ") failed" << std::endl;
                            printSchedule(processors_schedule);
                        }
                        return false;
                    }
                    processor_usage[i] = task_chosen.n->wcet;
                    processors_schedule[i].push_back(task_chosen.n);
                }
                else
                {
                    if (processors_schedule[i].empty() || processors_schedule[i].back()->groupId != 888)
                    {
                        auto idleNode = std::make_shared<Node>();
                        idleNode->groupId = 888; //idle task
                        idleNode->bcet = delta;
                        idleNode->wcet = delta;
                        idleNode->name = "idle";
                        idleNode->shortName = "idle";
                        idleNode->uniqueId = ++last_uniqueId;
                        processors_schedule[i].push_back(idleNode);
                    }
                    else
                    {
                        processors_schedule[i].back()->wcet += delta;
                        processors_schedule[i].back()->bcet += delta;
                    }
                }
            }
        }
    }

    if (verbose)
    {
        std::cout << "Succeded!" << std::endl;
        printSchedule(processors_schedule);
    }
    if (filename != "")
        scheduleToTikz(filename, processors_schedule, dag.getPeriod());

    return true;
}

#endif /*SCHEDULING_H*/