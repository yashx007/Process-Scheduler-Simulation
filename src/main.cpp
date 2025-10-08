
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
    std::cout << "Select scheduling algorithm:\n";
    std::cout << "0: Round Robin\n1: FCFS\n2: SJF\n3: SRTF\n4: Multi-level Queue\n5: Multi-level Feedback\n6: Priority Preemptive\n";
    std::cout << "Enter choice: ";
    std::cin >> type;
    if (type == 0 || type == 4 || type == 5) {
        std::cout << "Enter time quantum (suggest 1): ";
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
