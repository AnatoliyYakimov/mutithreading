#include <iostream>
#include <pthread.h>
#include <memory>

#define NUM_THREADS 4
#define ARRAY_LENGTH 20
#define TEST_CASE_COUNT 5
using namespace std;

bool isSortedAscending(const int *array, const uint length, uint threadCount = 5);
void *isSortedThreadTask(void *args);


int testCases[TEST_CASE_COUNT][ARRAY_LENGTH] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        {0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 10, 10, 10, 10, 10, 15, 16, 17, 18, 19},
        {0, 1, 2, 3, 4, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9,  12, 13, 14, 15, 16, 17, 18, 19}
};

bool expectedResults[] = {true, true, false, false, false};


int main() {
    printf("   Test   |  actual  | expected \n");
    for (int i = 0; i < TEST_CASE_COUNT; ++i) {
        bool sorted = isSortedAscending(testCases[i], ARRAY_LENGTH, NUM_THREADS);
        printf("%10d|%10s|%10s\n", i + 1,
               sorted ? "true" : "false",
               expectedResults[i] ? "true" : "false");
    }
    return 0;
}


/**
 * Входные данные для потока.
 */
struct ThreadData {
    const int *array;
    uint from;
    uint to;
};

/**
 * Результат, возвращаемый потоком.
 */
struct ThreadReturn {
    bool isSorted;
};

/**
 * Параллельно проверят, что массив отсортирован по неубыванию
 * @param array массив
 * @param length длина массива
 * @return true, если массив отсортирован по неубыванию
 */
bool isSortedAscending(const int *array, const uint length, uint threadCount) {

    auto threads = make_unique<pthread_t[]>(threadCount);
    auto args = make_unique<ThreadData[]>(threadCount);

    uint step = length / threadCount;
    uint border = 0;
    for (int i = 0; i < threadCount; ++i) {
        args[i].array = array;
        args[i].from = border;
        border += (i != threadCount - 1) ? step : length - border;
        args[i].to = border;
        int status = pthread_create(&(threads[i]), nullptr, &isSortedThreadTask,
                                    (void *) &args[i]);
        if (status != 0) {
            printf("Error while creating thread. Status: %d", status);
            exit(1);
        }
    }

    bool sorted = true;
    int previousElement = numeric_limits<int>::min();
    for (uint i = 0; i < NUM_THREADS && sorted; ++i) {
        ThreadReturn *threadReturn;
        int returnCode = pthread_join(threads[i], (void **) &threadReturn);
        if (returnCode != 0) {
            printf("Error while joining thread. Status %d", returnCode);
            exit(1);
        }
        if (threadReturn == nullptr) {
            printf("Thread didn't return any value");
            exit(1);
        }
        auto &threadArgs = args[i];
        int nextElement = array[threadArgs.from];
        if (threadReturn->isSorted && nextElement >= previousElement) {
            previousElement = array[threadArgs.to - 1];
        } else {
            sorted = false;
            for (uint j = i + 1; j < NUM_THREADS; ++j) {
                pthread_detach(threads[j]);
            }
        }
        delete threadReturn;
    }
    return sorted;
}

/**
 * Проверяет, что часть массива отсортирована
 * @param args значение типа ThreadData
 * @return значение типа ThreadReturn
 */
void *isSortedThreadTask(void *args) {
    auto &data = *((ThreadData *) args);
    auto&[array, from, to] = data;
    bool sorted = true;
    for (uint i = from + 1; i < to && sorted; ++i) {
        sorted = array[i] >= array[i - 1];
    }
    auto *ret = new ThreadReturn{sorted};
    pthread_exit((void *) ret);
}
