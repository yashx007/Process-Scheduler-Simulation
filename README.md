
# Process Scheduler Simulation

This project simulates CPU scheduling algorithms using C++ and POSIX threads. It was built as an educational simulator to demonstrate how different scheduling policies behave (Gantt charts, wait/turnaround calculations, and simple process threads).

The branch `feat/new-schedulers` adds multiple algorithms (FCFS, SJF, SRTF, Multi-level Queue, Multi-level Feedback) on top of the original Round Robin and Priority (preemptive) schedulers.

## Features

- Simulated processes represented by a `Process` structure
- Simulated execution using POSIX threads and condition variables
- Algorithms implemented:
	- Round Robin (RR)
	- First-Come, First-Serve (FCFS)
	- Shortest Job First (SJF) — non-preemptive
	- Shortest Remaining Time First (SRTF) — preemptive
	- Priority Scheduling (preemptive)
	- Multi-level Queue (foreground RR, background FCFS)
	- Multi-level Feedback Queue (3-level feedback queue)
- Text-based Gantt chart and per-process wait/turnaround statistics

## Design & Implementation Notes

- Each `Process` has fields: pid, arrival_time, burst_time, priority, remaining_time, start/finish times, wait/turnaround, and a dedicated thread plus synchronization primitives.
- The simulator uses a discrete time model where 1 simulated time unit ≈ 0.1s (implemented with `usleep(100000)`). This keeps the simulation visible and deterministic for demonstration.
- Scheduler implementations drive the simulation by signaling the process threads to 'run' for one time unit at a time (or multiple units for non-preemptive runs).

## File Layout

- `src/main.cpp` — interactive launcher and menu
- `src/scheduler.h` — `Process` struct, `Scheduler` class and algorithm declarations
- `src/scheduler.cpp` — implementations of scheduling policies and simulation loop
- `.github/copilot-instructions.md` — automation notes

## Build Instructions

You can build on Linux, macOS, or Windows (MSYS2 MinGW64). Use a terminal that has `g++` available.

On Windows (MSYS2 MinGW64), open "MSYS2 MinGW 64-bit" and run:

```bash
cd /c/Users/HP/Desktop/Process_Scheduler
g++ -std=c++11 -pthread src/main.cpp src/scheduler.cpp -o scheduler.exe
```

On Linux/macOS:

```bash
cd /path/to/Process_Scheduler
g++ -std=c++11 -pthread src/main.cpp src/scheduler.cpp -o scheduler
```

## Run Instructions

Run the program in the same directory where you built it:

```bash
./scheduler.exe   # Windows/MSYS2
./scheduler       # Linux/macOS
```

The program runs interactively. It will prompt for number of processes, each process's arrival time, burst time and priority, then it will ask you to select a scheduling algorithm. For RR, Multi-level Queue and MLFQ you will be asked for a time quantum.

Example session:

```
Enter number of processes: 3
Process 1 arrival time: 0
Process 1 burst time: 5
Process 1 priority: 1
Process 2 arrival time: 1
Process 2 burst time: 3
Process 2 priority: 6
Process 3 arrival time: 2
Process 3 burst time: 2
Process 3 priority: 2
Select scheduling algorithm:
0: Round Robin
1: FCFS
2: SJF
3: SRTF
4: Multi-level Queue
5: Multi-level Feedback
6: Priority Preemptive
Enter choice: 2

Gantt Chart:
P1 P2 P3 P2 P1 ...

Process  Wait Time   Turnaround Time
1        ...         ...
2        ...         ...
3        ...         ...

Average Wait Time: ...
Average Turnaround Time: ...
```

## Input / Output for Automated Testing

- The project is primarily interactive. If you need non-interactive testing, consider redirecting input from a file or adding a small wrapper that reads input from `input.txt` and writes output to `output.txt`.

