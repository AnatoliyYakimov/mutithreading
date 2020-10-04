/**
 * Дерево бинарное. Параллельный обход дерева.
 * В каждом узле хранится длинный массив, который проверяем на упорядоченность.
 * Посчитать число узлов, которых зранят упорядоченные масивы
 * Свободный узел отдаётся потоку. Левый и правый - потоку.
 * В свою очередь каждый отдаёт своих братьев потоку.
 * Всего имеется 4 потока. Если все потоки заняты, то ждём освободившихся потоков.
 * Ближайший освободившийся поток получает работу. 
 */

#include "lab5/threadmanager.h"
#include "lab5/binarytree.h"

#include <random>
#include <iostream>

using namespace std;

random_device rd;
mt19937 gen(rd());
mutex coutLock;

vector<int> createSorted(size_t count);
vector<int> createUnsorted(size_t count);

int main() {

    auto *tree = new BinaryTree<int, vector<int>>();

    const double sortedChance = 0.5;

    size_t total = 100000;
    size_t elementCount = 10000;
    size_t sortedCount = 0;

    cout << "Initializing tree...\n";
    cout << "Nodes in tree: " << total << endl;
    cout << "Elements in node: " << elementCount << endl;
    cout << "Chance, that node is sorted: " << sortedChance << endl;

    uniform_real_distribution<> probabilityDist(0, 1);
    uniform_int_distribution<> keyDist(0, total);
    for (int i = 0; i < total; ++i) {
        int key = keyDist(gen);
        vector<int> vec;
        if (probabilityDist(gen) > sortedChance) {
            vec = createSorted(elementCount);
            sortedCount++;
        } else {
            vec = createUnsorted(elementCount);
        }
        tree->add(key, vec);
    }

    cout << "Tree initialization complete\n";
    cout << "Initializing thread manager\n";

    ThreadManager threadManager(1000);

    atomic<size_t> actualSortedCount = 0;
    atomic<size_t> tasksCompleted = 0;

    uniform_int_distribution<> sleepDist(100, 500);
    tree->trace([&threadManager, &actualSortedCount, &tasksCompleted, &sleepDist](int &key,
                                                                 vector<int> &value) {
        auto task = function([&value, &actualSortedCount, &tasksCompleted, &sleepDist]() -> void {
            this_thread::sleep_for(chrono::milliseconds(sleepDist(gen)));
            bool isSorted = true;
            for (int i = 1; i < value.size() && isSorted; ++i) {
                isSorted = value[i - 1] <= value[i];
            }
            if (isSorted) {
                actualSortedCount++;
            }
            tasksCompleted++;
        });

        threadManager.schedule(task);
    });

    while (!(tasksCompleted == total)) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "Expected sorted arrays: " << sortedCount << "\n";
    cout << "Actual sorted arrays: " << actualSortedCount << "\n";
    threadManager.shutdown();
}

/**
 * Создаёт массив случайных элементов, которые гарантированно не отсортированы по возрастанию.
 */
vector<int> createUnsorted(size_t count) {
    uniform_int_distribution<> dist(1, 100);
    vector<int> vec(count);
    vec[0] = dist(gen);
    for (int i = 1; i < count - 1; ++i) {
        vec[i] = dist(gen);
    }
    uniform_int_distribution<> randomIndex(1, count);
    size_t idx = randomIndex(gen);
    vec[idx] = vec[idx - 1] - 1;
    return move(vec);
}

/**
 * Создаёт массив случайных элементов, которые гарантированно отсортированы по возрастанию.
 */
vector<int> createSorted(size_t count) {
    uniform_int_distribution<> dist(1, 100);
    vector<int> vec(count);
    vec[0] = dist(gen);
    for (int i = 1; i < count; ++i) {
        vec[i] = vec[i - 1] + dist(gen);
    }
    return move(vec);
}
