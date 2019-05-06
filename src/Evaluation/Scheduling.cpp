#include "Scheduling.h"

bool compareSchedulingInfos(const std::shared_ptr<ScheduleInfos> a, const std::shared_ptr<ScheduleInfos> b)
{
    return ((a->est > b->est) || (a->est == b->est && a->lft > b->lft));
}

bool compareSchedulingInfosESTdescending(const std::shared_ptr<ScheduleInfos> a, const std::shared_ptr<ScheduleInfos> b)
{
    return (a->est < b->est);
}

void printSchedule(const std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule)
{
    for (size_t i = 0; i < processors_schedule.size(); i++)
    {
        std::cout << "---Processor: " << i << "\t";
        for (const auto &n : processors_schedule[i])
            std::cout << n->uniqueId << "(" << n->wcet << "); ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void scheduleToTikz(const std::string &filename,
                    const std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule,
                    const double period)
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
    for (const auto &p : processors_schedule)
    {
        tikz_file << "\\draw (0," << y << ") --(0," << y + 1 << ") ;" << std::endl;
        tikz_file << "\\draw[<-] (" << period + 0.2 << "," << y << ") ->(" << period + 0.2 << ","
                  << y + 1 << ") ;" << std::endl;
        tikz_file << "\\draw (-0.2," << y << ") --(" << period + 0.5 << "," << y << ") ;"
                  << std::endl;
        tikz_file << "\\draw (0-0.5," << y + 0.5 << ") node {$P_{" << proc << "}$};" << std::endl;

        x = 0;
        for (const auto &n : p)
        {
            if (n->groupId != 888)
                tikz_file << "\\draw (" << x << "," << y << ") rectangle (" << x + n->wcet << ","
                          << y + 0.7 << ") node[pos=.5] {$" << n->shortName << "$};" << std::endl;
            tikz_file << "\\draw (" << x << "," << y << ") node[below] {$" << x << "$};"
                      << std::endl;
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

ScheduleInfosVec
createScheduleInfos(const DAG &dag, unsigned &last_uniqueId, bool verbose)
{
    ScheduleInfosVec nodes_to_sched;
    auto nodes = dag.getNodes();
    auto infos = dag.getNodeInfo();

    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->groupId != 0 && nodes[i]->groupId != 667 && nodes[i]->groupId != 666)
        {
            std::shared_ptr<ScheduleInfos> si = std::make_shared<ScheduleInfos>();
            si->n = nodes[i];
            si->est = infos.est[i];
            si->lst = infos.lst[i];
            si->eft = infos.eft[i];
            si->lft = infos.lft[i];
            nodes_to_sched.push_back(si);
        }
        if (i == nodes.size() - 1)
            last_uniqueId = nodes[i]->uniqueId;
    }

    auto edges = dag.getEdges();
    for (size_t i = 0; i < edges.size(); i++)
    {
        for (size_t j = 0; j < nodes_to_sched.size(); j++)
        {
            if (nodes_to_sched[j]->n->uniqueId == edges[i].from->uniqueId)
            {
                for (size_t k = 0; k < nodes_to_sched.size(); k++)
                {
                    if (nodes_to_sched[k]->n->uniqueId == edges[i].to->uniqueId)
                    {
                        nodes_to_sched[k]->pred.push_back(nodes_to_sched[j]);
                        nodes_to_sched[j]->succ.push_back(nodes_to_sched[k]);
                    }
                }
            }
        }
    }
    
    std::sort(nodes_to_sched.begin(), nodes_to_sched.end(), compareSchedulingInfos);

    if (verbose)
    {
        std::cout << "ID\tWCET\tBCET\tEST\tLST\tEFT\tEFT" << std::endl;
        for (const auto &n : nodes_to_sched)
        {
            std::cout << "\t" << n->n->uniqueId << "\t" << n->n->wcet << "\t" << n->n->bcet << "\t" << n->est
                      << "\t" << n->lst << "\t" << n->eft << "\t" << n->eft << std::endl;
        }
    }

    return nodes_to_sched;
}

void scheduleIdleTask(const int proc, std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule, const double delta, unsigned &last_uniqueId)
{
    if (processors_schedule[proc].empty() || processors_schedule[proc].back()->groupId != 888)
    {
        auto idleNode = std::make_shared<Node>();
        idleNode->groupId = 888; //idle task
        idleNode->bcet = delta;
        idleNode->wcet = delta;
        idleNode->name = "idle";
        idleNode->shortName = "idle";
        idleNode->uniqueId = ++last_uniqueId;
        processors_schedule[proc].push_back(idleNode);
    }
    else
    {
        processors_schedule[proc].back()->wcet += delta;
        processors_schedule[proc].back()->bcet += delta;
    }
}
void scheduleTask(const std::shared_ptr<ScheduleInfos> &task, int proc, std::vector<double> &processor_usage, ScheduleInfosVec &nodes_to_sched, ScheduleInfosVec &ready, std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule, const double t, const double epsilon)
{
    processor_usage[proc] = task->n->wcet;
    processors_schedule[proc].push_back(task->n);

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
    
    /*for (const auto &e : dag.getEdges())
    {
        if (e.from == task->n)
        {
            for (auto &ntd : nodes_to_sched)
            {
                if (ntd->n == e.to && t + task->n->wcet > ntd->est)
                {
                    //std::cout<<"change from "<<ntd->est;
                    ntd->est = std::max(t + task->n->wcet, ntd->est);
                    change = true;
                    //std::cout<<"to : "<< ntd->est<<std::endl;
                }
            }
            for (auto &r : ready)
            {
                if (r->n == e.to && t + task->n->wcet > r->est)
                {
                    //std::cout<<"---------change from "<<r->est;
                    r->est = std::max(t + task->n->wcet, r->est);
                    //std::cout<<"to : "<< r->est<<std::endl;
                    change = true;
                }
            }
        }
    }*/

    if (change)
    {
        std::sort(ready.begin(), ready.end(), compareSchedulingInfosESTdescending);
        while (!ready.empty() && std::isgreater(ready.back()->est - epsilon, t))
        {
            nodes_to_sched.push_back(ready.back());
            ready.pop_back();
        }
        std::sort(nodes_to_sched.begin(), nodes_to_sched.end(), compareSchedulingInfos);
    }
}

double computeDelta(const std::vector<double> &processor_usage, const ScheduleInfosVec &nodes_to_sched, const ScheduleInfosVec &ready, const double t, const double epsilon)
{
    double delta = 1000;
    auto max_ut = *(std::max_element(std::begin(processor_usage), std::end(processor_usage)));
    if (std::isgreater(max_ut, epsilon))
        delta = max_ut;
    for (const auto &r : ready)
        if (std::isless(r->n->wcet, delta))
            delta = r->n->wcet;
    for (const auto &p : processor_usage)
        if (std::isgreater(p, 0) && std::isless(p, delta))
            delta = p;
    if (!nodes_to_sched.empty())
        delta = std::isless(delta, nodes_to_sched.back()->est - t) ? delta : nodes_to_sched.back()->est - t;

    if (std::isless(delta, epsilon))
    {
        std::cout << "Something is VERY WRONG" << std::endl;
        return false;
    }

    //delta = 0.1;
    return delta;
}

bool scheduleDAG(const DAG &dag, const unsigned n_proc, const bool verbose,
                 const std::string &filename)
{
    unsigned last_uniqueId;

    ScheduleInfosVec nodes_to_sched = createScheduleInfos(dag, last_uniqueId);
    ScheduleInfosVec ready;

    std::vector<std::vector<std::shared_ptr<Node>>> processors_schedule(n_proc);
    std::vector<double> processor_usage(n_proc, 0);

    double epsilon = 1e-5;
    double delta;

    for (double t = 0; t < dag.getPeriod() && !(ready.empty() && nodes_to_sched.empty()); t +=
                                                                                          delta)
    {
        //create ready queue
        while (!nodes_to_sched.empty() && std::islessequal(nodes_to_sched.back()->est, t + epsilon))
        {
            ready.push_back(nodes_to_sched.back());
            nodes_to_sched.pop_back();
        }
        std::sort(ready.begin(), ready.end(), compareSchedulingInfos);

        //update processor state
        for (size_t i = 0; i < n_proc; i++)
        {
            if (std::isgreater(processor_usage[i], 0))
            {
                processor_usage[i] -= delta;
                if (std::isless(processor_usage[i], epsilon))
                    processor_usage[i] = 0;
            }
        }

        // compute delta: next point in time to check
        delta = computeDelta(processor_usage, nodes_to_sched, ready, t, epsilon);

        //if a processor is avaiable and there are ready tasks, schedule them, otherwise schedule an idle task
        for (size_t i = 0; i < n_proc; i++)
        {
            if (std::abs(processor_usage[i]) <= epsilon)
            {
                if (!ready.empty())
                {
                    auto task_chosen = ready.back();
                    ready.pop_back();

                    //check if there is a deadline miss
                    if (std::isgreater(t + task_chosen->n->wcet, task_chosen->lft + epsilon))
                    {
                        if (verbose)
                        {
                            std::cout << "Failed at t = " << t << "; ";
                            std::cout << "task actual ft:" << t + task_chosen->n->wcet
                                      << " task lft:" << task_chosen->lft << "; ";
                            std::cout << task_chosen->n->uniqueId << "(" << task_chosen->n->wcet
                                      << ") failed" << std::endl;
                            printSchedule(processors_schedule);
                        }
                        
                        return false;
                    }
                    //if not, schedule task
                    scheduleTask(task_chosen, i, processor_usage, nodes_to_sched, ready, processors_schedule, t, epsilon);
                }
                else
                {
                    //schedule idel task
                    scheduleIdleTask(i, processors_schedule, delta, last_uniqueId);
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
