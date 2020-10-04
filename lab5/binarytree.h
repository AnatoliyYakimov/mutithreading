
#ifndef POSIX_THREADS_BINARYTREE_H
#define POSIX_THREADS_BINARYTREE_H

using namespace std;

#include "node.h"

template<typename K, typename V>
class BinaryTree {

public:

    void add(const K &key, const V &value);

    void trace(const function<void(K&, V&)> &callback);

    void trace(Node<K, V> *node, const function<void(K&, V&)> &callback);

    virtual ~BinaryTree();

private:
    Node<K, V> *root;

    void add(const K &key, const V &value, Node<K, V> *node);

};

template<typename K, typename V>
void BinaryTree<K, V>::add(const K &key, const V &value) {
    if (!root) {
        root = new Node<K, V>(key, value);
    } else {
        add(key, value, root);
    }
}

template<typename K, typename V>
void BinaryTree<K, V>::add(const K &key, const V &value, Node<K, V> *node) {
    if (key > node->key) {
        if (node->right) {
            add(key, value, node->right);
        } else {
            node->right = new Node<K, V>(key, value);
        }
    } else {
        if (node->left) {
            add(key, value, node->left);
        } else {
            node->left = new Node<K, V>(key, value);
        }
    }
}

template<typename K, typename V>
void BinaryTree<K, V>::trace(const function<void(K&, V&)> &callback) {
    trace(root, callback);
}

template<typename K, typename V>
void BinaryTree<K, V>::trace(Node<K, V> *node,
                             const function<void(K&, V&)> &callback) {
    callback(node->key, node->value);
    if (node->left) {
        trace(node->left, callback);
    }
    if (node->right) {
        trace(node->right, callback);
    }
}

template<typename K, typename V>
BinaryTree<K, V>::~BinaryTree() {
    delete root;
}


#endif //POSIX_THREADS_BINARYTREE_H
