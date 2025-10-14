#!/bin/bash
# Simple test runner for Process Scheduler Simulation
# Usage: ./run_tests.sh

PROJ_ROOT=$(cd "$(dirname "$0")/.." && pwd)
BIN="$PROJ_ROOT/scheduler.exe"
if [ ! -x "$BIN" ]; then
  echo "Binary $BIN not found. Build the project first."; exit 1
fi

for f in sample*.txt; do
  echo "---- Running $f ----"
  cat "$f" | "$BIN"
  echo
done
