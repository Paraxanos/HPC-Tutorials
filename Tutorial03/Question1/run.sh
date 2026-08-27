#!/usr/bin/env bash
# Q1: builds both programs, runs the full points x threads grid and writes the
# CSVs that plot.py turns into the required graphs.
set -euo pipefail
cd "$(dirname "$0")"

REPS=${REPS:-3}
POINTS=(10 100 10000 1000000 10000000 100000000 1000000000)
THREADS=(1 2 4 8 16)

echo "== building =="
gcc -O2 -Wall -Wextra -o q1_pi_serial   q1_pi_serial.c
gcc -O2 -Wall -Wextra -o q1_pi_pthreads q1_pi_pthreads.c -lpthread

# best <time-field-index> <command...> -> echoes the repetition with the smallest time
best() {
    local f=$1; shift
    local out="" line
    for ((r = 0; r < REPS; r++)); do
        line=$("$@")
        if [ -z "$out" ] || awk -v a="$(cut -d, -f"$f" <<<"$line")" \
             -v b="$(cut -d, -f"$f" <<<"$out")" 'BEGIN{exit !(a<b)}'; then
            out=$line
        fi
    done
    echo "$out"
}

echo "== serial =="
echo "points,pi,error,time" > q1_serial.csv
for n in "${POINTS[@]}"; do
    best 4 ./q1_pi_serial "$n" | tee -a q1_serial.csv
done

echo "== pthreads =="
echo "threads,points,pi,error,time" > q1_parallel.csv
for p in "${THREADS[@]}"; do
    for n in "${POINTS[@]}"; do
        best 5 ./q1_pi_pthreads "$n" "$p" | tee -a q1_parallel.csv
    done
done

echo "== sanity check: the estimate must land near pi for large N =="
awk -F, 'NR>1 && $2>=100000000 && $4>0.01 {print "FAIL: N="$2" error="$4; bad=1}
         END{ if (bad) exit 1; print "OK: every run with N >= 1e8 is within 0.01 of pi" }' q1_parallel.csv

echo "== plots =="
python3 plot.py
