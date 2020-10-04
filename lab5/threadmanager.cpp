
#include <iostream>
#include "threadmanager.h"

using namespace std;



ThreadManager::ThreadManager(int threadCount)
        :  threads() {
    auto daemonFactory = [this](int id) {
        return [this, id]() {
            while (!terminated) {
                {
                    lock_guard<mutex> lock(coutLock);
                    cout << "Thread " << id << ": waiting for task\n";
                }
                auto task = tasks.pop();
                {
                    lock_guard<mutex> lock(coutLock);
                    cout << "Thread " << id << ": task started\n";
                }
                task();
                {
                    lock_guard<mutex> lock(coutLock);
                    cout << "Thread " << id << ": task done\n";
                }
            }
        };
    };
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(daemonFactory(i + 1));
    }
}

void ThreadManager::terminate() {
    terminated = true;
    shutdown();
}

ThreadManager::~ThreadManager() {
    shutdown();
}

void ThreadManager::schedule(std::function<void(void)> callable) {
    tasks.push(callable);
}

void ThreadManager::shutdown() {
    for(int i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }
}
