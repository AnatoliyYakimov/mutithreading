//
// Created by yakimov on 10/4/20.
//

#ifndef POSIX_THREADS_MONITOR_H
#define POSIX_THREADS_MONITOR_H

#include <semaphore.h>
#include <mutex>
#include <atomic>
#include <functional>


using namespace std;

class Monitor {
public:
    Monitor();

    void wait(unique_lock<mutex>& lock, const function<bool(void)> &predicate);

    void notify();

    void notifyAll();

protected:
    void wait(unique_lock<mutex>& lock);

private:
    mutex mtx;
    atomic_uint waitersCount = 0;
    sem_t sem;
};


#endif //POSIX_THREADS_MONITOR_H
