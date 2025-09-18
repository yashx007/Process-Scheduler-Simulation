// Scheduler implementation
#include "scheduler.h"
#include <iostream>
#include <algorithm>
#include <unistd.h>

Scheduler::Scheduler(SchedulingType t, int q) : type(t), quantum(q) {
    pthread_mutex_init(&mutex, nullptr);
}

void Scheduler::addProcess(const Process& p) {
    processes.push_back(p);
}

void Scheduler::run() {
    if (type == ROUND_ROBIN) {
        roundRobin();
    } else {
        priorityPreemptive();
    }
}

void Scheduler::printGanttChart() {
    std::cout << "\nGantt Chart:\n";
    for (const auto& entry : gantt_chart) {
        std::cout << entry << " ";
    }
    std::cout << std::endl;
}

void Scheduler::printStats() {
    double total_wait = 0, total_turnaround = 0;
    std::cout << "\nProcess\tWait Time\tTurnaround Time\n";
    for (const auto& p : processes) {
        std::cout << p.pid << "\t" << p.wait_time << "\t\t" << p.turnaround_time << std::endl;
        total_wait += p.wait_time;
        total_turnaround += p.turnaround_time;
    }
    std::cout << "\nAverage Wait Time: " << total_wait / processes.size() << std::endl;
    std::cout << "Average Turnaround Time: " << total_turnaround / processes.size() << std::endl;
}

void Scheduler::roundRobin() {
    int time = 0;
    int completed = 0;
    std::vector<Process*> ready;
    while (completed < processes.size()) {
        for (auto& p : processes) {
            if (p.arrival_time <= time && p.remaining_time > 0 && std::find(ready.begin(), ready.end(), &p) == ready.end()) {
                ready.push_back(&p);
            }
        }
        if (ready.empty()) {
            time++;
            continue;
        }
        for (auto it = ready.begin(); it != ready.end();) {
            Process* p = *it;
            int exec_time = std::min(quantum, p->remaining_time);
            gantt_chart.push_back("P" + std::to_string(p->pid));
            p->remaining_time -= exec_time;
            time += exec_time;
            if (p->remaining_time == 0) {
                p->finish_time = time;
                p->turnaround_time = p->finish_time - p->arrival_time;
                p->wait_time = p->turnaround_time - p->burst_time;
                completed++;
                it = ready.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Scheduler::priorityPreemptive() {
    int time = 0;
    int completed = 0;
    while (completed < processes.size()) {
        std::vector<Process*> ready;
        for (auto& p : processes) {
            if (p.arrival_time <= time && p.remaining_time > 0) {
                ready.push_back(&p);
            }
        }
        if (ready.empty()) {
            time++;
            continue;
        }
        auto cmp = [](Process* a, Process* b) { return a->priority < b->priority; };
        std::sort(ready.begin(), ready.end(), cmp);
        Process* p = ready.front();
        gantt_chart.push_back("P" + std::to_string(p->pid));
        p->remaining_time--;
        time++;
        if (p->remaining_time == 0) {
            p->finish_time = time;
            p->turnaround_time = p->finish_time - p->arrival_time;
            p->wait_time = p->turnaround_time - p->burst_time;
            completed++;
        }
    }
}
