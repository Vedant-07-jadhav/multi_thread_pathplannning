#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
class ThreadPool {
private:
  std::vector<std::thread> workers;

  std::queue<std::function<void()>> tasks;

  std::mutex queue_mutex;
  std::condition_variable condition;

  bool stop;

public:
  ThreadPool(size_t num_threads);
  ~ThreadPool();
  void enqueue(std::function<void()> task);
};

#endif