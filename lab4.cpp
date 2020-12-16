#include <iostream>
#include <future>
#include <random>
#include "lab4/stack.h"

using namespace std;

mutex coutLock;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist(1000, 2000);

int main() {
    LockingStack<int> stack;

    auto pushCallback = [&stack](int id, int count) {
        for (int i = 0; i < count; ++i) {
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));
            int value = id * count * 1000 + i;
            stack.push(value);
//            lock_guard<mutex> lock(coutLock);

        }
    };

    auto popCallback = [&stack](int id, int count) {
        for (int i = 0; i < count; ++i) {
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));
            int val = stack.pop();
            lock_guard<mutex> lock(coutLock);
            cout << "Consumer " << id << ": pop " << val << endl;
        }
    };

    vector<future<void>> producers;
    vector<future<void>> consumers;

    int count = 20;

    for (int i = 1; i < 3; ++i) {
        producers.push_back(async(pushCallback, i, count));
        consumers.push_back(async(popCallback, i, count));
    }

    for (int i = 0; i < 2; ++i) {
        producers[i].get();
        consumers[i].get();
    }

    this_thread::sleep_for(chrono::seconds(1));
    auto vec = stack.getInternalVector();
    for_each(vec.begin(), vec.end(), [](int val) {
        cout << val << " ";
    });
}
