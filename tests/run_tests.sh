#!/bin/bash
# Simple test runner for Process Scheduler Simulation
# Usage: ./run_tests.sh

PROJ_ROOT=$(cd "$(dirname "$0")/.." && pwd)
done
BIN="$PROJ_ROOT/scheduler.exe"
if [ ! -x "$BIN" ]; then
  echo "Binary $BIN not found. Build the project first."; exit 1
fi

for f in sample*.txt; do
  echo "---- Running $f ----"
  out=$(cat "$f" | "$BIN")
  echo "$out"
  # Basic assertions: output should contain Gantt Chart and Process header
  echo "$out" | grep -q "Gantt Chart:" && echo "Gantt Chart found" || { echo "Gantt Chart missing"; exit 2; }
  echo "$out" | grep -q "Process" && echo "Process table found" || { echo "Process table missing"; exit 3; }
  echo "PASS: $f"
  echo
done
