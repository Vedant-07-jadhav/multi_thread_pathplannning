# Multithreaded Path Planning Engine

> A high-performance multithreaded A\* path planning system designed to benchmark how graph search scales across CPU cores.

The project implements an optimized A\* solver and executes **thousands of independent path planning tasks in parallel** using a custom thread pool. It also includes a benchmarking framework that evaluates parallel scalability across different grid sizes and thread counts.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [A\* Algorithm](#a-algorithm)
- [Performance Optimizations](#performance-optimizations)
- [Benchmark Experiments](#benchmark-experiments)
- [Build & Run](#build--run)
- [Plotting Results](#plotting-results)
- [Applications](#applications)
- [Future Improvements](#future-improvements)
- [License](#license)

---

## Overview

Path planning is a core problem in robotics navigation, autonomous vehicles, game AI, and multi-agent simulation. When many agents require paths simultaneously, computing them sequentially becomes a bottleneck.

This project demonstrates how **task-level parallelism** can accelerate large batches of path planning queries.

---

## Features

| Feature | Description |
|---|---|
| **Multithreaded Execution** | Runs thousands of path searches in parallel using a custom thread pool |
| **Optimized A\* Implementation** | Flat memory layout, integer costs, visit-ID trick, and thread-local workspace reuse |
| **Benchmark Harness** | Automatically runs experiments across grid sizes, thread counts, and workloads |
| **Data Collection** | Results written to `results.jsonl` for downstream analysis |

---

## Architecture

```
             Task Queue
                  │
                  ▼
            Thread Pool
    ┌──────────┬──────────┬──────────┐
    ▼          ▼          ▼          ▼
 Worker1    Worker2    Worker3    Worker4
    │          │          │          │
 Workspace  Workspace  Workspace  Workspace
    │
    ▼
  A* Solver
    │
    ▼
   Grid
```

**Shared data** — the grid is read-only and shared across all workers.

**Thread-local data** — each worker owns its own search buffers, avoiding race conditions and eliminating locking during search:

```
Workspace
├── g_cost
├── visit_id
└── parent
```

---

## A\* Algorithm

The solver uses the standard A\* search algorithm with the cost function:

```
f(n) = g(n) + h(n)
```

where `g(n)` is the cost from the start node, and `h(n)` is the heuristic estimate to the goal.

The heuristic used is **Manhattan distance**:

```
h = |x1 - x2| + |y1 - y2|
```

This heuristic is **admissible and consistent**, guaranteeing optimal paths on a 4-connected grid.

---

## Performance Optimizations

### Visit-ID 

Instead of resetting search arrays between queries with an O(grid\_size) loop:

```cpp
// Slow: clears entire array each search
for (all cells) reset g_cost;
```

Each node is considered visited only if its recorded ID matches the current search ID:

```cpp
visit_id[node] == current_search_id
```

This eliminates **O(grid\_size)** memory resets per search.

### Flat Memory Layout

Data structures use contiguous arrays (`std::vector<int>`) instead of nested vectors, yielding improved cache locality, faster memory access, and reduced pointer overhead.

### Thread-Local Workspaces

Each worker thread maintains its own workspace:

```cpp
thread_local Workspace ws;
```

This avoids repeated heap allocation and eliminates shared mutable state entirely.

---

## Benchmark Experiments

Experiments sweep across the following parameters:

| Parameter | Values |
|---|---|
| **Grid sizes** | 100×100, 200×200, 300×300, 500×500 |
| **Thread counts** | 1, 2, 4, 8 |
| **Searches per experiment** | 1,000 |

### Metrics Collected

Each experiment records the following fields in `results.jsonl`:

| Field | Description |
|---|---|
| `runtime_seconds` | Total wall-clock time |
| `avg_time_per_search` | Mean time per path query |
| `paths_per_second` | Throughput |
| `total_nodes_expanded` | Aggregate node expansions |
| `avg_nodes_per_search` | Mean nodes expanded per query |
| `total_path_length` | Sum of all returned path lengths |

**Example entry:**

```json
{
  "grid": 500,
  "threads": 8,
  "searches": 1000,
  "runtime": 0.45,
  "avg_time": 0.00045,
  "paths_per_second": 2220,
  "avg_nodes": 47494
}
```

### Example Result

```
Grid size : 500 × 500
Searches  : 1,000
Threads   : 8
Runtime   : 0.45 seconds
Throughput: ~2,200 paths/second
```

Compared to a single-threaded run (~4 seconds), this achieves roughly **8.8× speedup**, demonstrating strong parallel scalability.

### Parallel Scaling

Two graphs are generated from benchmark results:

**Runtime vs. Threads** - runtime decreases as more cores are used.

**Speedup** - real performance compared against ideal linear scaling:

```
Speedup = T1 / Tn
```

---

## Build & Run

**Compile:**

```bash
g++ -O2 main.cpp astar.cpp grid.cpp thread_pool.cpp -o main
```

**Run:**

```bash
./main
```

This generates `results.jsonl` with all benchmark data.

---

## Plotting Results

**Install dependencies:**

```bash
pip install pandas matplotlib
```

**Generate plots:**

```bash
python plot_results.py
```

This produces graphs for runtime scaling, speedup, and throughput.

---

## Applications

This system demonstrates techniques used in:

- Robotics navigation
- Autonomous vehicle planning
- Game AI pathfinding
- High-performance graph search
- Parallel task engines

---

## Future Improvements

- Jump Point Search (JPS)
- Bidirectional A\*
- Hierarchical pathfinding
- GPU-accelerated search
- Visualization of search frontier

---

## License
