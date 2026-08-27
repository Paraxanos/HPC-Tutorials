#!/usr/bin/env bash
# Q2: builds both programs, times the serial run and 1..16 threads on the same
# image, checks the parallel output is identical, then draws the graphs.
set -euo pipefail
cd "$(dirname "$0")"

REPS=${REPS:-3}
THREADS=(1 2 4 8 16)
N_IMG=${N_IMG:-8192}

echo "== building =="
gcc -O2 -Wall -Wextra -o q2_edge_serial   q2_edge_serial.c
gcc -O2 -Wall -Wextra -o q2_edge_pthreads q2_edge_pthreads.c -lpthread

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

echo "== serial (n = $N_IMG) =="
echo "n,time" > q2_serial.csv
best 2 ./q2_edge_serial "$N_IMG" | tee -a q2_serial.csv

echo "== pthreads =="
echo "threads,n,time" > q2_parallel.csv
for p in "${THREADS[@]}"; do
    best 3 ./q2_edge_pthreads "$N_IMG" "$p" | tee -a q2_parallel.csv
done

echo "== correctness check: parallel output must equal serial output =="
./q2_edge_serial   512    edge_serial.pgm > /dev/null
./q2_edge_pthreads 512 16 edge_16t.pgm    > /dev/null
cmp edge_serial.pgm edge_16t.pgm && echo "OK: 16-thread output is identical to serial"

echo "== plots =="
python3 plot.py
