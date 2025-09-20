
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
    if (type == ROUND_ROBIN) {
        roundRobin();
    } else {
        priorityPreemptive();
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
