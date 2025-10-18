This folder contains sample input files and a small runner to exercise the scheduler in non-interactive mode.

sample1.txt / sample2.txt / sample3.txt
- Each file encodes process count and per-process lines with "arrival burst priority" on separate lines. After the process block, two extra numbers are provided (algorithm choice, quantum) so the runner can feed the interactive program.

run_tests.sh
- Make it executable and run from the `tests` folder after building the project binary in the project root.

Example:

```bash
cd Process_Scheduler/tests
../scheduler.exe < sample1.txt
```
