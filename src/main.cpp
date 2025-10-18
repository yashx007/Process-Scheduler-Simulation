
#include "scheduler.h"
#include <iostream>
#include <string>

// Simple CLI: interactive by default. If program is invoked with --batch or
// if stdin is redirected, it reads a compact format from stdin:
// n
// arrival burst priority  (repeated n times)
// algorithm_number
// quantum (optional)

int main(int argc, char** argv) {
    bool batch = false;
    for (int i = 1; i < argc; ++i) if (std::string(argv[i]) == "--batch" || std::string(argv[i]) == "-b") batch = true;

    int n, type, quantum = 1;
    std::vector<Process> plist;

    // Heuristic: if batch flag provided or stdin is not a terminal, use batch mode
    bool stdin_is_tty = isatty(fileno(stdin));
    if (batch || !stdin_is_tty) {
        if (!(std::cin >> n)) return 1;
        for (int i = 0; i < n; ++i) {
            Process p; p.pid = i + 1;
            std::cin >> p.arrival_time >> p.burst_time >> p.priority;
            p.remaining_time = p.burst_time;
            plist.push_back(p);
        }
        if (!(std::cin >> type)) return 1;
        if (type == 0 || type == 4 || type == 5) {
            if (!(std::cin >> quantum)) quantum = 1;
        }
    } else {
        std::cout << "Enter number of processes: ";
        std::cin >> n;
        for (int i = 0; i < n; ++i) {
            Process p; p.pid = i + 1;
            std::cout << "Process " << p.pid << " arrival time: "; std::cin >> p.arrival_time;
            std::cout << "Process " << p.pid << " burst time: "; std::cin >> p.burst_time;
            std::cout << "Process " << p.pid << " priority: "; std::cin >> p.priority;
            p.remaining_time = p.burst_time;
            plist.push_back(p);
        }
        std::cout << "Select scheduling algorithm:\n";
        std::cout << "0: Round Robin\n1: FCFS\n2: SJF\n3: SRTF\n4: Multi-level Queue\n5: Multi-level Feedback\n6: Priority Preemptive\n";
        std::cout << "Enter choice: "; std::cin >> type;
        if (type == 0 || type == 4 || type == 5) {
            std::cout << "Enter time quantum (suggest 1): "; std::cin >> quantum;
        }
    }

    Scheduler scheduler(static_cast<SchedulingType>(type), quantum);
    for (const auto& p : plist) scheduler.addProcess(p);
    scheduler.run();
    scheduler.printGanttChart();
    scheduler.printStats();
    return 0;
}
