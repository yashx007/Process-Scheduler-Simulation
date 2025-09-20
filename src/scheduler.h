
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include <pthread.h>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int finish_time;
    int wait_time;
    int turnaround_time;
    pthread_t thread;
    pthread_mutex_t proc_mutex;
    pthread_cond_t proc_cond;
    bool is_running = false;
    bool is_completed = false;
};

enum SchedulingType {
    ROUND_ROBIN,
    PRIORITY_PREEMPTIVE
};

class Scheduler {
public:
    Scheduler(SchedulingType type, int quantum = 1);
    void addProcess(const Process& p);
    void run();
    void printGanttChart();
    void printStats();
private:
    std::vector<Process> processes;
    SchedulingType type;
    int quantum;
    pthread_mutex_t mutex;
    std::vector<std::string> gantt_chart;
    void roundRobin();
    void priorityPreemptive();
    static void* processThreadFunc(void* arg);
};

#endif 
