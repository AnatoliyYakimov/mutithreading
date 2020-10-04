//
// Created by yakimov on 10/2/20.
//

#ifndef POSIX_THREADS_THREADMANAGER_H
#define POSIX_THREADS_THREADMANAGER_H


#include <functional>
#include <stack.h>
#include <thread>

class ThreadManager {
public:
    explicit ThreadManager(int threadCount);

    void schedule(std::function<void(void)> callable);

    void terminate();

    void shutdown();

    virtual ~ThreadManager();

private:
    std::vector<std::thread> threads;
    LockingStack<std::function<void()>> tasks;
    std::atomic<bool> terminated = false;
    mutex coutLock;
};



#endif //POSIX_THREADS_THREADMANAGER_H
