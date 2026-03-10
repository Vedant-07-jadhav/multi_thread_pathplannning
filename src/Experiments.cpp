#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "astar.h"
#include "grid.h"
#include "thread_pool.h"

void run_experiment(
    int grid_size,
    int threads,
    int searches,
    std::ofstream &out)
{
    Grid grid(grid_size, grid_size);
    grid.generateRandomObstacles(0.2);

    std::vector<std::vector<int>> gridData(
        grid_size,
        std::vector<int>(grid_size));

    for(int y = 0; y < grid_size; y++)
        for(int x = 0; x < grid_size; x++)
            gridData[y][x] = grid.isObstacle(x,y) ? 1 : 0;

    AStar solver(gridData);

    ThreadPool pool(threads);

    std::atomic<int> completed(0);
    std::atomic<long long> nodesExpanded(0);
    std::atomic<long long> totalPathLength(0);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dx(0,grid_size-1);
    std::uniform_int_distribution<> dy(0,grid_size-1);

    auto startTime = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < searches; i++)
    {
        Node start{dx(gen), dy(gen)};
        Node goal{dx(gen), dy(gen)};

        pool.enqueue([&, start, goal]
        {
            thread_local Workspace ws(grid_size * grid_size);

            Result r = solver.solve(start, goal, ws);

            nodesExpanded += r.nodesExpanded;
            totalPathLength += r.path.size();

            completed++;
        });
    }

    while(completed < searches)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    auto endTime = std::chrono::high_resolution_clock::now();

    double runtime =
        std::chrono::duration<double>(endTime - startTime).count();

    double avgTime = runtime / searches;
    double pathsPerSec = searches / runtime;
    double avgNodes = (double)nodesExpanded / searches;

    out << "{"
        << "\"grid\":" << grid_size << ","
        << "\"threads\":" << threads << ","
        << "\"searches\":" << searches << ","
        << "\"runtime\":" << runtime << ","
        << "\"avg_time\":" << avgTime << ","
        << "\"paths_per_second\":" << pathsPerSec << ","
        << "\"total_nodes\":" << nodesExpanded << ","
        << "\"avg_nodes\":" << avgNodes << ","
        << "\"path_length\":" << totalPathLength
        << "}\n";

    std::cout << "Finished grid=" << grid_size
              << " threads=" << threads
              << " runtime=" << runtime << "s\n";
}

int main()
{
    std::ofstream out("results.jsonl");

    std::vector<int> grids = {100,200,300,500};
    std::vector<int> threads = {1,2,4,8};

    int searches = 1000;

    for(int g : grids)
    {
        for(int t : threads)
        {
            run_experiment(g,t,searches,out);
        }
    }

    std::cout << "Benchmark complete. Results saved to results.jsonl\n";

    return 0;
}