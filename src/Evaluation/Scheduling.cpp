#include "Scheduling.h"

bool scheduling::compareSchedulingInfo(const std::shared_ptr<ScheduleInfo> a, const std::shared_ptr<ScheduleInfo> b)
{
    return ((a->est > b->est) || (a->est == b->est && a->lft > b->lft));
}

bool scheduling::compareSchedulingInfoESTdescending(const std::shared_ptr<ScheduleInfo> a, const std::shared_ptr<ScheduleInfo> b)
{
    return (a->est < b->est);
}

void scheduling::printSchedule(const std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule)
{
    for (size_t i = 0; i < processorSchedule.size(); i++)
    {
        std::cout << "---Processor: " << i << "\t";
        for (const auto &n : processorSchedule[i])
            std::cout << n->uniqueId << "(" << n->wcet << "); ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void scheduling::scheduleToTikz(const std::string &filename,
                    const std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule,
                    const double period)
{
    //opening the tex file
    std::ofstream tikzFile;
    tikzFile.open(filename);

    //beginning the tikz figure
    tikzFile << "\\documentclass[tikz,border=10pt]{standalone}\n"
                 "\\usepackage{tkz-graph}\n"
                 "\\usetikzlibrary{automata}\n"
                 "\\usetikzlibrary[automata]\n"
                 "\\begin{document}\n";

    tikzFile << "\\begin{tikzpicture}\n";

    double x = 0, y = 0;
    int proc = 1;
    for (const auto &p : processorSchedule)
    {
        tikzFile << "\\draw (0," << y << ") --(0," << y + 1 << ") ;" << std::endl;
        tikzFile << "\\draw[<-] (" << period + 0.2 << "," << y << ") ->(" << period + 0.2 << ","
                  << y + 1 << ") ;" << std::endl;
        tikzFile << "\\draw (-0.2," << y << ") --(" << period + 0.5 << "," << y << ") ;"
                  << std::endl;
        tikzFile << "\\draw (0-0.5," << y + 0.5 << ") node {$P_{" << proc << "}$};" << std::endl;

        x = 0;
        for (const auto &n : p)
        {
            if (n->groupId != 888)
                tikzFile << "\\draw (" << x << "," << y << ") rectangle (" << x + n->wcet << ","
                          << y + 0.7 << ") node[pos=.5] {$" << n->shortName << "$};" << std::endl;
            tikzFile << "\\draw (" << x << "," << y << ") node[below] {$" << x << "$};"
                      << std::endl;
            x += n->wcet;
        }

        y += 2;
        proc++;
    }

    //ending tikz figure
    tikzFile << "\\end{tikzpicture}\n";
    tikzFile << "\\end{document}\n";

    //close the file
    tikzFile.close();
}

scheduling::ScheduleInfoVec
scheduling::createScheduleInfo(const DAG &dag, unsigned &lastUniqueId, bool verbose)
{
    ScheduleInfoVec nodesToSched;
    auto nodes = dag.getNodes();
    auto info = dag.getNodeInfo();

    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->groupId != 668 && nodes[i]->groupId != 667 && nodes[i]->groupId != 666)
        {
            std::shared_ptr<ScheduleInfo> si = std::make_shared<ScheduleInfo>();
            si->n = nodes[i];
            si->est = info.est[i];
            si->lst = info.lst[i];
            si->eft = info.eft[i];
            si->lft = info.lft[i];
            nodesToSched.push_back(si);
        }
        if (i == nodes.size() - 1)
            lastUniqueId = nodes[i]->uniqueId;
    }

    auto edges = dag.getEdges();
    for (size_t i = 0; i < edges.size(); i++)
    {
        for (size_t j = 0; j < nodesToSched.size(); j++)
        {
            if (nodesToSched[j]->n->uniqueId == edges[i].from->uniqueId)
            {
                for (size_t k = 0; k < nodesToSched.size(); k++)
                {
                    if (nodesToSched[k]->n->uniqueId == edges[i].to->uniqueId)
                    {
                        nodesToSched[k]->pred.push_back(nodesToSched[j]);
                        nodesToSched[j]->succ.push_back(nodesToSched[k]);
                    }
                }
            }
        }
    }
    
    std::sort(nodesToSched.begin(), nodesToSched.end(), compareSchedulingInfo);

    if (verbose)
    {
        std::cout << "ID\tWCET\tBCET\tEST\tLST\tEFT\tEFT" << std::endl;
        for (const auto &n : nodesToSched)
        {
            std::cout << "\t" << n->n->uniqueId << "\t" << n->n->wcet << "\t" << n->n->bcet << "\t" << n->est
                      << "\t" << n->lst << "\t" << n->eft << "\t" << n->eft << std::endl;
        }
    }

    return nodesToSched;
}

void scheduling::scheduleIdleTask(const int proc, std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule, const double delta, unsigned &lastUniqueId)
{
    if (processorSchedule[proc].empty() || processorSchedule[proc].back()->groupId != 888)
    {
        auto idleNode = std::make_shared<Node>();
        idleNode->groupId = 888; //idle task
        idleNode->bcet = delta;
        idleNode->wcet = delta;
        idleNode->name = "idle";
        idleNode->shortName = "idle";
        idleNode->uniqueId = ++lastUniqueId;
        processorSchedule[proc].push_back(idleNode);
    }
    else
    {
        processorSchedule[proc].back()->wcet += delta;
        processorSchedule[proc].back()->bcet += delta;
    }
}
void scheduling::scheduleTask(const std::shared_ptr<ScheduleInfo> &task, int proc, std::vector<double> &processorUsage, ScheduleInfoVec &nodesToSched, ScheduleInfoVec &ready, std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule, const double t, const double epsilon)
{
    processorUsage[proc] = task->n->wcet;
    processorSchedule[proc].push_back(task->n);

    //fix precedence constraints
    bool change = false;
    for (auto &s : task->succ)
    {
        if (std::isgreater(t + task->n->wcet, s->est + epsilon))
        {
            s->est = std::max(t + task->n->wcet, s->est);
            change = true;
        }
    }

    if (change)
    {
        std::sort(ready.begin(), ready.end(), compareSchedulingInfoESTdescending);
        while (!ready.empty() && std::isgreater(ready.back()->est - epsilon, t))
        {
            nodesToSched.push_back(ready.back());
            ready.pop_back();
        }
        std::sort(nodesToSched.begin(), nodesToSched.end(), compareSchedulingInfo);
    }
}

double scheduling::computeDelta(const std::vector<double> &processorUsage, const ScheduleInfoVec &nodesToSched, const ScheduleInfoVec &ready, const double t, const double epsilon)
{
    double delta = 1000;
    auto maxUt = *(std::max_element(std::begin(processorUsage), std::end(processorUsage)));
    if (std::isgreater(maxUt, epsilon))
        delta = maxUt;
    for (const auto &r : ready)
        if (std::isless(r->n->wcet, delta))
            delta = r->n->wcet;
    for (const auto &p : processorUsage)
        if (std::isgreater(p, 0) && std::isless(p, delta))
            delta = p;
    if (!nodesToSched.empty())
        delta = std::isless(delta, nodesToSched.back()->est - t) ? delta : nodesToSched.back()->est - t;

    if (std::isless(delta, epsilon))
    {
        std::cout << "Something is VERY WRONG" << std::endl;
        return 0.1;
    }

    //delta = 0.1;
    return delta;
}

bool scheduling::scheduleDAG(const DAG &dag, const unsigned nProc,const std::string &filename, const bool verbose)
{
    unsigned lastUniqueId;

    ScheduleInfoVec nodesToSched = createScheduleInfo(dag, lastUniqueId);
    ScheduleInfoVec ready;

    std::vector<std::vector<std::shared_ptr<Node>>> processorSchedule(nProc);
    std::vector<double> processorUsage(nProc, 0);

    double epsilon = 1e-5;
    double delta;

    for (double t = 0; t < dag.getPeriod() && !(ready.empty() && nodesToSched.empty()); t +=
                                                                                          delta)
    {
        //create ready queue
        while (!nodesToSched.empty() && std::islessequal(nodesToSched.back()->est, t + epsilon))
        {
            ready.push_back(nodesToSched.back());
            nodesToSched.pop_back();
        }
        std::sort(ready.begin(), ready.end(), compareSchedulingInfo);

        //update processor state
        for (size_t i = 0; i < nProc; i++)
        {
            if (std::isgreater(processorUsage[i], 0))
            {
                processorUsage[i] -= delta;
                if (std::isless(processorUsage[i], epsilon))
                    processorUsage[i] = 0;
            }
        }

        // compute delta: next point in time to check
        delta = computeDelta(processorUsage, nodesToSched, ready, t, epsilon);

        //if a processor is avaiable and there are ready tasks, schedule them, otherwise schedule an idle task
        for (size_t i = 0; i < nProc; i++)
        {
            if (std::islessequal(processorUsage[i],epsilon))
            {
                if (!ready.empty())
                {
                    auto taskChosen = ready.back();
                    ready.pop_back();

                    //check if there is a deadline miss
                    if (std::isgreater(t + taskChosen->n->wcet, taskChosen->lft + epsilon))
                    {
                        if (verbose)
                        {
                            std::cout << "Failed at t = " << t << "; ";
                            std::cout << "task actual ft:" << t + taskChosen->n->wcet
                                      << " task lft:" << taskChosen->lft << "; ";
                            std::cout << taskChosen->n->uniqueId << "(" << taskChosen->n->wcet
                                      << ") failed" << std::endl;
                            printSchedule(processorSchedule);
                        }
                        
                        return false;
                    }
                    //if not, schedule task
                    scheduleTask(taskChosen, i, processorUsage, nodesToSched, ready, processorSchedule, t, epsilon);
                }
                else
                {
                    //schedule idel task
                    scheduleIdleTask(i, processorSchedule, delta, lastUniqueId);
                }
            }
        }
    }

    if (verbose)
    {
        std::cout << "Succeded!" << std::endl;
        printSchedule(processorSchedule);
    }
    if (filename != "")
        scheduleToTikz(filename, processorSchedule, dag.getPeriod());
    
    return true;
}
