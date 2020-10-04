#include <iostream>
#include <threads.h>
#include <memory>
#include <thread>
#include <future>
#include <vector>
#include <cmath>

#define NUM_THREADS 4
#define VECTOR_LENGTH 20
#define TEST_CASE_COUNT 5
using namespace std;

bool isSorted(const vector<int> &vec);

const vector<int> testCases[TEST_CASE_COUNT] = {
        {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                18, 19
        },
        {
                0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 10, 10, 10, 10, 10, 15, 16, 17,
                18, 19
        },
        {
                0, 1, 2, 3, 4, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                18, 19
        },
        {
                2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                18, 19
        },
        {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 12, 13, 14, 15, 16, 17, 18,
                19
        }
};

bool expectedResults[] = {true, true, false, false, false};

int main() {
    printf("   Test   |  actual  | expected \n");
    for (int i = 0; i < TEST_CASE_COUNT; ++i) {
        bool sorted = isSorted(testCases[i]);
        printf("%10d|%10s|%10s\n", i + 1,
               sorted ? "true" : "false",
               expectedResults[i] ? "true" : "false");
    }
    return 0;
}

bool isSorted(const vector<int> &vec) {
    size_t step = ceil((1.0 * vec.size()) / NUM_THREADS);
    vector<size_t> starts;
    starts.push_back(0);

    for (size_t i = 1; i < NUM_THREADS; ++i) {
        starts.push_back(starts[i - 1] + step);
    }
    starts.push_back(vec.size());
    for (int i = 1; i < starts.size() - 1; ++i) {
        int left = vec[starts[i] - 1];
        int right = vec[starts[i]];
        if (left > right) {
            return false;
        }
    }

    auto func = [&vec](uint from, uint to) -> bool {
        bool sorted = true;
        for (uint i = from + 1; i < to && sorted; ++i) {
            sorted = vec[i] >= vec[i - 1];
        }
        return sorted;
    };

    vector<future<bool>> futures;
    for (int i = 0; i < NUM_THREADS; ++i) {
        future<bool> future = async(launch::async, func, starts[i],
                                    starts[i + 1]);
        futures.push_back(move(future));
    }

    for (auto &future : futures) {
        if (!future.get()) {
            return false;
        }
    }
    return true;
}






