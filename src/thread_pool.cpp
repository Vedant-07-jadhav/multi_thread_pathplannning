#include "thread_pool.h"
#include <cstddef>
#include <cstdio>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>

ThreadPool::ThreadPool(size_t num_threads)
    : stop(false)
{
    for (size_t i = 0; i < num_threads; i++) {
        workers.emplace_back([this] { // create a worker thread
            while (true) { // loop to keep the thread alive and waiting for tasks
                std::function<void()> task; // task to be executed
                {
                    std::unique_lock<std::mutex> lock(
                        this->queue_mutex); // lock the queue mutex to access the task
                                            // queue

                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    }); // wait until there is a task to execute or the thread pool is
                        // stopped

                    if (this->stop && this->tasks.empty())
                        return; // if the thread pool is stopped and there are no tasks
                                // left, exit the thread

                    task = std::move(
                        this->tasks.front()); // get the next task from the queue
                    this->tasks.pop(); // remove the task from the queue
                }

                task(); // execute the task
            }
        });
    }
}

void ThreadPool::enqueue(
    std::function<void()> task)
{ // add a new task to the queue
    {
        std::unique_lock<std::mutex> lock(
            queue_mutex); // lock the queue mutex to access the task queue
        tasks.push(std::move(task)); // add the task to the queue
    }

    condition.notify_one(); // notify one worker thread that there is a new task
                            // to execute
}

ThreadPool::~ThreadPool()
{

    {
        std::unique_lock<std::mutex> lock(
            queue_mutex); // lock the queue mutex to access the task queue
        stop = true; // set the stop flag to true to indicate that the thread pool
                     // is being destroyed
    }
    condition.notify_all(); // notify all worker threads to wake up and exit

    for (std::thread& worker : workers)
        worker.join(); // wait for all worker threads to finish executing before
                       // destroying the thread pool
}
