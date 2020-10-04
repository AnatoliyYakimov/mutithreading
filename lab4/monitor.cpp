
#include "monitor.h"

Monitor::Monitor() {
    sem_init(&sem, 0, 0);
}

void Monitor::notify() {
    lock_guard<mutex> lock(mtx);
    if (waitersCount > 0) {
        sem_post(&sem);
        waitersCount--;
    }
}

void Monitor::notifyAll() {
    lock_guard<mutex> lock(mtx);
    for (int i = 0; i < waitersCount; ++i) {
        sem_post(&sem);
    }
    waitersCount = 0;
}

void Monitor::wait(unique_lock<mutex> &lock, const function<bool(void)> &predicate) {
    while (!predicate()) {
        wait(lock);
    }
}

void Monitor::wait(unique_lock<mutex> &lock){
    lock.unlock();
    {
        lock_guard<mutex> internalLock(mtx);
        waitersCount++;
    }
    sem_wait(&sem);
    lock.lock();
}
