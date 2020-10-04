
#ifndef POSIX_THREADS_NODE_H
#define POSIX_THREADS_NODE_H

using namespace std;

template<typename K, typename V>
class Node {
public:
    Node(K key, V value)
            : key(key), value(value), left(nullptr), right(nullptr) {}

    K key;
    V value;
    Node<K,V> *left;
    Node<K,V> *right;

    virtual ~Node() {
        delete left, right;
    }
};
#endif //POSIX_THREADS_NODE_H
