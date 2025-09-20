
#include "scheduler.h"
#include <iostream>

int main() {
    int n, type, quantum = 1;
    std::cout << "Enter number of processes: ";
    std::cin >> n;
    std::vector<Process> plist;
    for (int i = 0; i < n; ++i) {
        Process p;
        p.pid = i + 1;
        std::cout << "Process " << p.pid << " arrival time: ";
        std::cin >> p.arrival_time;
        std::cout << "Process " << p.pid << " burst time: ";
        std::cin >> p.burst_time;
        std::cout << "Process " << p.pid << " priority: ";
        std::cin >> p.priority;
        p.remaining_time = p.burst_time;
        plist.push_back(p);
    }
    std::cout << "Select scheduling algorithm (0: Round Robin, 1: Priority Preemptive): ";
    std::cin >> type;
    if (type == 0) {
        std::cout << "Enter time quantum: ";
        std::cin >> quantum;
    }
    Scheduler scheduler(static_cast<SchedulingType>(type), quantum);
    for (const auto& p : plist) {
        scheduler.addProcess(p);
    }
    scheduler.run();
    scheduler.printGanttChart();
    scheduler.printStats();
    return 0;
}
