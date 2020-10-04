

#ifndef POSIX_THREADS_STACK_H
#define POSIX_THREADS_STACK_H


#include <vector>
#include <atomic>
#include <cassert>
#include <mutex>
#include <condition_variable>
#include "semaphore.h"
#include "monitor.h"

/**
 * Бинарный семафор
 * @tparam T
 */
template<typename T>
class LockingStack {
public:
    LockingStack() {
        assert(sem_init(&producerSem, 0, 1) == 0);
        assert(sem_init(&consumerSem, 0, 0) == 0);
    }

    T pop();

    void push(T value);

    const std::vector<T> &getInternalVector() const;

    virtual ~LockingStack();

protected:
    mutex mtx;
    sem_t producerSem;
    sem_t consumerSem;
    Monitor monitor;
    std::vector<T> *internalVector = new std::vector<T>();
};

template<typename T>
void LockingStack<T>::push(T value) {
    unique_lock<mutex> lock(mtx);
    internalVector->push_back(move(value));
    monitor.notify();
}

template<typename T>
T LockingStack<T>::pop() {
    unique_lock<mutex> lock(mtx);
    monitor.wait(lock, [this]{return internalVector->size() > 0;});
    T value = (*internalVector)[internalVector->size() - 1];
    internalVector->pop_back();
    return move(value);
}

template<typename T>
const std::vector<T> &LockingStack<T>::getInternalVector() const {
    return *internalVector;
}

template<typename T>
LockingStack<T>::~LockingStack() {
    delete internalVector;
}


#endif //POSIX_THREADS_STACK_H
