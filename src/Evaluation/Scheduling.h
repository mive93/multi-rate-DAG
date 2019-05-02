#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "DAG/DAG.h"
#include "DAG/Node.h"
#include <vector>
#include <iostream>

struct ScheduleInfos
{
    std::shared_ptr<Node> n;
    unsigned est;
    unsigned lst;
    unsigned eft;
    unsigned lft;
};

bool compareSchedulingInfos(ScheduleInfos &a, ScheduleInfos &b)
{
    return ((a.est > b.est) || (a.est == b.est && a.lst > b.lst));
}

void printSchedule(std::vector<std::vector<std::shared_ptr<Node>>> processors_schedule)
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

bool scheduleDAG(DAG dag, unsigned n_proc)
{

    std::vector<ScheduleInfos> nodes_to_sched;

    auto nodes = dag.getNodes();
    auto infos = dag.getNodeInfo();
    unsigned last_uniqueId;
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

    std::vector<std::vector<std::shared_ptr<Node>>> processors_schedule(n_proc);
    std::vector<float> processor_usage(n_proc, 0);
    std::vector<ScheduleInfos> ready;

    float t = 0;
    float delta = 1;
    for (; t < dag.getPeriod() && !(ready.empty() && nodes_to_sched.empty()); t += delta)
    {

        while (!nodes_to_sched.empty() && nodes_to_sched.back().est <= t)
        {
            ready.push_back(nodes_to_sched.back());
            nodes_to_sched.pop_back();
        }

        std::sort(ready.begin(), ready.end(), compareSchedulingInfos);

        for (size_t i = 0; i < n_proc; i++)
        {
            if (processor_usage[i] > 0)
            {
                processor_usage[i] -= delta;
                if (processor_usage[i] < 0)
                    processor_usage[i] = 0;
            }
        }

        delta = *(std::max_element(std::begin(processor_usage), std::end(processor_usage)));
        for (auto p : processor_usage)
            if (p > 0 && p < delta)
                delta = p;

        delta = (delta < nodes_to_sched.back().est - t) ? delta : nodes_to_sched.back().est - t;
        //delta = 1;

        for (size_t i = 0; i < n_proc; i++)
        {
            if (processor_usage[i] == 0)
            {
                if (!ready.empty())
                {
                    auto task_chosen = ready.back();
                    ready.pop_back();
                    if (t + task_chosen.n->wcet > task_chosen.lft)
                    {

                        std::cout << "Failed at t = " << t << "; ";
                        std::cout << "task actual ft:" << t + task_chosen.n->wcet << " task lft:" << task_chosen.lft << "; ";
                        std::cout << task_chosen.n->uniqueId << "(" << task_chosen.n->wcet << ") failed" << std::endl;
                        printSchedule(processors_schedule);
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
                        idleNode->groupId = 888;
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

    /*for (size_t i = 0; i < n_proc; i++)
        std::cout << "processor: " << i << " usage: " << processor_usage[i] << std::endl;*/
    std::cout << "Succeded!"<<std::endl;
    printSchedule(processors_schedule);

    return true;
}

#endif /*SCHEDULING_H*/