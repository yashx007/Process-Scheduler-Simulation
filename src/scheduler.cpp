
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
    
    for (auto& p : processes) {
        pthread_mutex_init(&p.proc_mutex, nullptr);
        pthread_cond_init(&p.proc_cond, nullptr);
        p.is_running = false;
        p.is_completed = false;
        pthread_create(&p.thread, nullptr, Scheduler::processThreadFunc, &p);
    }
    switch (type) {
        case ROUND_ROBIN: roundRobin(); break;
        case FCFS: fcfs(); break;
        case SJF: sjf(); break;
        case SRTF: srtf(); break;
        case MULTI_LEVEL_QUEUE: multiLevelQueue(); break;
        case MULTI_LEVEL_FEEDBACK: multiLevelFeedback(); break;
        case PRIORITY_PREEMPTIVE: priorityPreemptive(); break;
        default: roundRobin(); break;
    }
    // Wait for all threads to finish
    for (auto& p : processes) {
        pthread_join(p.thread, nullptr);
        pthread_mutex_destroy(&p.proc_mutex);
        pthread_cond_destroy(&p.proc_cond);
    }
}
// Thread function for process simulation
void* Scheduler::processThreadFunc(void* arg) {
    Process* p = static_cast<Process*>(arg);
    while (!p->is_completed) {
        pthread_mutex_lock(&p->proc_mutex);
        while (!p->is_running && !p->is_completed) {
            pthread_cond_wait(&p->proc_cond, &p->proc_mutex);
        }
        if (p->is_completed) {
            pthread_mutex_unlock(&p->proc_mutex);
            break;
        }
        // Simulate execution (sleep for 1 unit)
        usleep(100000); 
        p->is_running = false;
        pthread_mutex_unlock(&p->proc_mutex);
    }
    return nullptr;
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
            // Signal process thread to run for exec_time units
            for (int i = 0; i < exec_time; ++i) {
                pthread_mutex_lock(&p->proc_mutex);
                p->is_running = true;
                pthread_cond_signal(&p->proc_cond);
                pthread_mutex_unlock(&p->proc_mutex);
                // Wait for simulated execution
                usleep(100000); // 0.1 sec per time unit
                p->remaining_time--;
                time++;
            }
            if (p->remaining_time == 0) {
                p->finish_time = time;
                p->turnaround_time = p->finish_time - p->arrival_time;
                p->wait_time = p->turnaround_time - p->burst_time;
                p->is_completed = true;
                pthread_mutex_lock(&p->proc_mutex);
                pthread_cond_signal(&p->proc_cond);
                pthread_mutex_unlock(&p->proc_mutex);
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
        // Signal process thread to run for 1 unit (preemptive)
        pthread_mutex_lock(&p->proc_mutex);
        p->is_running = true;
        pthread_cond_signal(&p->proc_cond);
        pthread_mutex_unlock(&p->proc_mutex);
        usleep(100000); 
        p->remaining_time--;
        time++;
        if (p->remaining_time == 0) {
            p->finish_time = time;
            p->turnaround_time = p->finish_time - p->arrival_time;
            p->wait_time = p->turnaround_time - p->burst_time;
            p->is_completed = true;
            pthread_mutex_lock(&p->proc_mutex);
            pthread_cond_signal(&p->proc_cond);
            pthread_mutex_unlock(&p->proc_mutex);
            completed++;
        }
    }
}

// First-Come First-Serve (non-preemptive)
void Scheduler::fcfs() {
    int time = 0;
    // sort by arrival time
    std::vector<Process*> order;
    for (auto &p : processes) order.push_back(&p);
    std::sort(order.begin(), order.end(), [](Process* a, Process* b){
        return a->arrival_time < b->arrival_time;
    });
    for (auto p : order) {
        if (time < p->arrival_time) time = p->arrival_time;
        gantt_chart.push_back("P" + std::to_string(p->pid));
        // run until completion
        while (p->remaining_time > 0) {
            pthread_mutex_lock(&p->proc_mutex);
            p->is_running = true;
            pthread_cond_signal(&p->proc_cond);
            pthread_mutex_unlock(&p->proc_mutex);
            usleep(100000);
            p->remaining_time--;
            time++;
        }
        p->finish_time = time;
        p->turnaround_time = p->finish_time - p->arrival_time;
        p->wait_time = p->turnaround_time - p->burst_time;
        p->is_completed = true;
    }
}

// Shortest Job First (non-preemptive)
void Scheduler::sjf() {
    int time = 0, completed = 0;
    size_t n = processes.size();
    while (completed < n) {
        // find ready processes
        std::vector<Process*> ready;
        for (auto &p : processes) if (p.arrival_time <= time && p.remaining_time > 0) ready.push_back(&p);
        if (ready.empty()) { time++; continue; }
        std::sort(ready.begin(), ready.end(), [](Process* a, Process* b){ return a->burst_time < b->burst_time; });
        Process* p = ready.front();
        gantt_chart.push_back("P" + std::to_string(p->pid));
        // run to completion
        while (p->remaining_time > 0) {
            pthread_mutex_lock(&p->proc_mutex);
            p->is_running = true;
            pthread_cond_signal(&p->proc_cond);
            pthread_mutex_unlock(&p->proc_mutex);
            usleep(100000);
            p->remaining_time--;
            time++;
        }
        p->finish_time = time;
        p->turnaround_time = p->finish_time - p->arrival_time;
        p->wait_time = p->turnaround_time - p->burst_time;
        p->is_completed = true;
        completed++;
    }
}

// Shortest Remaining Time First (preemptive SJF)
void Scheduler::srtf() {
    int time = 0, completed = 0;
    size_t n = processes.size();
    while (completed < n) {
        std::vector<Process*> ready;
        for (auto &p : processes) if (p.arrival_time <= time && p.remaining_time > 0) ready.push_back(&p);
        if (ready.empty()) { time++; continue; }
        std::sort(ready.begin(), ready.end(), [](Process* a, Process* b){ return a->remaining_time < b->remaining_time; });
        Process* p = ready.front();
        gantt_chart.push_back("P" + std::to_string(p->pid));
        pthread_mutex_lock(&p->proc_mutex);
        p->is_running = true;
        pthread_cond_signal(&p->proc_cond);
        pthread_mutex_unlock(&p->proc_mutex);
        usleep(100000);
        p->remaining_time--;
        time++;
        if (p->remaining_time == 0) {
            p->finish_time = time;
            p->turnaround_time = p->finish_time - p->arrival_time;
            p->wait_time = p->turnaround_time - p->burst_time;
            p->is_completed = true;
            completed++;
        }
    }
}

// Multi-level queue: queue 0 is RR with quantum, queue 1 is FCFS
void Scheduler::multiLevelQueue() {
    int time = 0, completed = 0;
    size_t n = processes.size();
    std::vector<Process*> q0, q1;
    while (completed < n) {
        for (auto &p : processes) {
            if (p.arrival_time <= time && p.remaining_time > 0) {
                // simple classification: priority < 5 => foreground
                if (p.priority < 5) {
                    if (std::find(q0.begin(), q0.end(), &p) == q0.end()) q0.push_back(&p);
                } else {
                    if (std::find(q1.begin(), q1.end(), &p) == q1.end()) q1.push_back(&p);
                }
            }
        }
        if (!q0.empty()) {
            Process* p = q0.front(); q0.erase(q0.begin());
            int exec = std::min(quantum, p->remaining_time);
            for (int i = 0; i < exec; ++i) {
                gantt_chart.push_back("P" + std::to_string(p->pid));
                pthread_mutex_lock(&p->proc_mutex);
                p->is_running = true;
                pthread_cond_signal(&p->proc_cond);
                pthread_mutex_unlock(&p->proc_mutex);
                usleep(100000);
                p->remaining_time--; time++;
            }
            if (p->remaining_time > 0) q0.push_back(p); else { p->is_completed = true; completed++; p->finish_time = time; p->turnaround_time = p->finish_time - p->arrival_time; p->wait_time = p->turnaround_time - p->burst_time; }
        } else if (!q1.empty()) {
            Process* p = q1.front(); q1.erase(q1.begin());
            // run to completion FCFS
            while (p->remaining_time > 0) {
                gantt_chart.push_back("P" + std::to_string(p->pid));
                pthread_mutex_lock(&p->proc_mutex);
                p->is_running = true;
                pthread_cond_signal(&p->proc_cond);
                pthread_mutex_unlock(&p->proc_mutex);
                usleep(100000);
                p->remaining_time--; time++;
            }
            p->is_completed = true; completed++; p->finish_time = time; p->turnaround_time = p->finish_time - p->arrival_time; p->wait_time = p->turnaround_time - p->burst_time;
        } else { time++; }
    }
}

// Multi-level feedback queue: 3 levels with increasing quantum
void Scheduler::multiLevelFeedback() {
    int time = 0, completed = 0;
    size_t n = processes.size();
    std::vector<Process*> levels[3];
    int quantums[3] = {quantum, quantum*2, quantum*4};
    while (completed < n) {
        for (auto &p : processes) {
            if (p.arrival_time <= time && p.remaining_time > 0) {
                bool found=false;
                for (int l=0;l<3;++l) if (std::find(levels[l].begin(), levels[l].end(), &p) != levels[l].end()) found=true;
                if (!found) levels[0].push_back(&p);
            }
        }
        bool did=false;
        for (int lvl=0; lvl<3 && !did; ++lvl) {
            if (levels[lvl].empty()) continue;
            Process* p = levels[lvl].front(); levels[lvl].erase(levels[lvl].begin());
            int exec = std::min(quantums[lvl], p->remaining_time);
            for (int i=0;i<exec;++i) {
                gantt_chart.push_back("P" + std::to_string(p->pid));
                pthread_mutex_lock(&p->proc_mutex);
                p->is_running = true;
                pthread_cond_signal(&p->proc_cond);
                pthread_mutex_unlock(&p->proc_mutex);
                usleep(100000);
                p->remaining_time--; time++;
            }
            if (p->remaining_time > 0) {
                if (lvl < 2) levels[lvl+1].push_back(p);
                else levels[lvl].push_back(p);
            } else { p->is_completed=true; completed++; p->finish_time=time; p->turnaround_time=p->finish_time-p->arrival_time; p->wait_time=p->turnaround_time-p->burst_time; }
            did=true;
        }
        if (!did) time++;
    }
}
