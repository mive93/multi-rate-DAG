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


namespace scheduling
{

    struct ScheduleInfo
    {
        std::shared_ptr<Node> n;
        double est;
        double lst;
        double eft;
        double lft;
        std::vector<std::shared_ptr<ScheduleInfo>> pred;
        std::vector<std::shared_ptr<ScheduleInfo>> succ;
    };

    using ScheduleInfoVec = std::vector<std::shared_ptr<ScheduleInfo>>;

    bool
    compareSchedulingInfo(const std::shared_ptr<ScheduleInfo> a,
            const std::shared_ptr<ScheduleInfo> b);

    bool
    compareSchedulingInfoESTdescending(const std::shared_ptr<ScheduleInfo> a,
            const std::shared_ptr<ScheduleInfo> b);

    void
    printSchedule(const std::vector<std::vector<std::shared_ptr<Node>>>& processorSchedule);

    void
    scheduleToTikz(const std::string &filename,
            const std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule,
            const double period);

    ScheduleInfoVec
    createScheduleInfo(const DAG &dag, unsigned &lastUniqueId, bool verbose = false);

    void
    scheduleIdleTask(const int proc,
            std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule, const double delta,
            unsigned &lastUniqueId);
    void
    scheduleTask(const std::shared_ptr<ScheduleInfo> &task, int proc,
            std::vector<double> &processorUsage, ScheduleInfoVec &nodesToSched,
            ScheduleInfoVec &ready,
            std::vector<std::vector<std::shared_ptr<Node>>> &processorSchedule, const double t,
            const double epsilon);

    double
    computeDelta(const std::vector<double> &processorUsage, const ScheduleInfoVec &nodesToSched,
            const ScheduleInfoVec &ready, const double t, const double epsilon);

    bool
    scheduleDAG(const DAG &dag, const unsigned nProc,
            const std::string &filename = "", const bool verbose = false);

}
#endif /*SCHEDULING_H*/
