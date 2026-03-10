#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <thread>

#include "astar.h"
#include "grid.h"
#include "thread_pool.h"

int main()
{
    int width = 500;
    int height = 500;

    int searches = 1000;

    Grid grid(width, height);
    grid.generateRandomObstacles(0.2);

    std::vector<std::vector<int>> gridData(height, std::vector<int>(width));

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            gridData[y][x] = grid.isObstacle(x,y) ? 1 : 0;
        }
    }

    AStar solver(gridData);

    int numThreads = std::min(8u, std::thread::hardware_concurrency());

    ThreadPool pool(numThreads);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dx(0,width-1);
    std::uniform_int_distribution<> dy(0,height-1);

    std::atomic<int> completedTasks(0);
    std::atomic<int> totalExpanded(0);
    std::atomic<int> totalPathLength(0);

    auto startTime = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < searches; i++)
    {
        Node start{dx(gen), dy(gen)};
        Node goal{dx(gen), dy(gen)};

        pool.enqueue([&, start, goal]
        {
            // each worker thread gets its own workspace
            thread_local Workspace ws(width * height);

            Result r = solver.solve(start, goal, ws);

            totalExpanded += r.nodesExpanded;
            totalPathLength += r.path.size();

            completedTasks++;
        });
    }

    while(completedTasks < searches)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = endTime - startTime;

    std::cout << "Grid size: " << width << " x " << height << "\n";
    std::cout << "Searches run: " << searches << "\n";
    std::cout << "Threads used: " << numThreads << "\n";

    std::cout << "Total time: " << duration.count() << " seconds\n";
    std::cout << "Average time per search: "
              << duration.count() / searches << " seconds\n";

    std::cout << "Total nodes expanded: " << totalExpanded << "\n";
    std::cout << "Total path length: " << totalPathLength << "\n";

    return 0;
}