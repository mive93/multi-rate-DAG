#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "DAG/DAG.h"
#include "DAG/Node.h"
#include <vector>
#include <iostream>
#include <iomanip>

#define COL_END "\033[0m"
#define COL_CYANB "\033[1;36m"
// Simple Timer
#define TIMER_START      \
    timespec start, end; \
    clock_gettime(CLOCK_MONOTONIC, &start);

#define TIMER_STOP_C(col)                                           \
    clock_gettime(CLOCK_MONOTONIC, &end);                           \
    double t_ns = ((double)(end.tv_sec - start.tv_sec) * 1.0e9 +    \
                   (double)(end.tv_nsec - start.tv_nsec)) /         \
                  1.0e6;                                            \
    std::cout << col << "Time:" << std::setw(16) << t_ns << " ms\n" \
              << COL_END;

#define TIMER_STOP TIMER_STOP_C(COL_CYANB)

struct ScheduleInfos
{
    std::shared_ptr<Node> n;
    double est;
    double lst;
    double eft;
    double lft;
    std::vector<std::shared_ptr<ScheduleInfos>> pred;
    std::vector<std::shared_ptr<ScheduleInfos>> succ;
};

using ScheduleInfosVec = std::vector<std::shared_ptr<ScheduleInfos>>;

bool 
compareSchedulingInfos(const std::shared_ptr<ScheduleInfos> a, const std::shared_ptr<ScheduleInfos> b);
bool 
compareSchedulingInfosESTdescending(const std::shared_ptr<ScheduleInfos> a, const std::shared_ptr<ScheduleInfos> b);
void 
printSchedule(const std::vector<std::vector<std::shared_ptr<Node>>>& processors_schedule);
void 
scheduleToTikz(const std::string &filename,
                    const std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule,
                    const double period);
ScheduleInfosVec
createScheduleInfos(const DAG &dag, unsigned &last_uniqueId, bool verbose = false);

void 
scheduleIdleTask(const int proc,
                      std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule,
                      const double delta, unsigned &last_uniqueId);
void 
scheduleTask(const std::shared_ptr<ScheduleInfos> &task, int proc,
                  std::vector<double> &processor_usage,
                  ScheduleInfosVec &nodes_to_sched,
                  ScheduleInfosVec &ready,
                  std::vector<std::vector<std::shared_ptr<Node>>> &processors_schedule,
                  const double t, const double epsilon);

double
computeDelta(const std::vector<double> &processor_usage,
             const ScheduleInfosVec &nodes_to_sched,
             const ScheduleInfosVec &ready,
             const double t, const double epsilon);

bool 
scheduleDAG(const DAG &dag, const unsigned n_proc, const bool verbose = false,
                const std::string &filename = "");

#endif /*SCHEDULING_H*/
