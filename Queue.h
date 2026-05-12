#ifndef QUEUE_H
#define QUEUE_H

#include <LinkedList.h>

template <class T> class Queue;

template <class T>
std::ostream &operator<<(std::ostream &os, const Queue<T> &q);

template <class T> class Queue {
    LinkedList<T> list;

public:
    void enqueue(T value) { list.append(value); }

    T dequeue() { return list.removeFirst(); }

    T peek() { return list[0]; }

    bool isEmpty() { return list.size() == 0; }

    int size() { return list.size(); }

    friend std::ostream &operator<< <>(std::ostream &os, const Queue<T> &q);

    friend struct Graph;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Queue<T> &q) {
    os << q.list;

    return os;
}

#endif