#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <iostream>

template <class T> class LinkedList;

template <class T>
std::ostream &operator<<(std::ostream &os, const LinkedList<T> &list);

template <class T> struct Link {
    T data;
    Link *next;
    Link *prev;

    Link() {
        data = 0;
        next = nullptr;
        prev = nullptr;
    }

    Link(T data) {
        this->data = data;
        next = nullptr;
        prev = nullptr;
    }
};

template <class T> class LinkedList {
protected:
    Link<T> *front;
    Link<T> *back;
    int count;

public:
    LinkedList() {
        front = nullptr;
        back = nullptr;
        count = 0;
    }

    LinkedList(const LinkedList &other) {
        front = nullptr;
        back = nullptr;
        count = 0;

        Link<T> *temp = other.front;
        while (temp != nullptr) {
            append(temp->data);
            temp = temp->next;
        }
    }

    LinkedList &operator=(const LinkedList &other) {
        while (count < other.count) {
            append(0);
        }

        while (count > other.count) {
            removeFirst();
        }

        Link<T> *ours = front;
        Link<T> *theirs = other.front;

        while (ours != nullptr) {
            ours->data = theirs->data;
            ours = ours->next;
            theirs = theirs->next;
        }

        return *this;
    }

    T &operator[](int index) {
        if (index < 0 || index >= count) {
            throw std::logic_error("Index out of bounds");
        }

        Link<T> *temp = front;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        return temp->data;
    }

    void append(T value) {
        Link<T> *newLink = new Link<T>(value);

        if (front == nullptr) {
            front = newLink;
            back = newLink;
        } else {
            newLink->prev = back;
            back->next = newLink;
            back = newLink;
        }
        count++;
    }

    void prepend(T value) {
        Link<T> *newLink = new Link<T>(value);

        if (front == nullptr) {
            front = newLink;
            back = newLink;
        } else {
            newLink->next = front;
            front->prev = newLink;
            front = newLink;
        }
        count++;
    }

    T removeFirst() {
        if (front == nullptr) {
            throw std::logic_error("List is empty");
        } else if (front == back) {
            T target = front->data;

            delete front;
            front = nullptr;
            back = nullptr;
            count--;

            return target;
        } else {
            T target = front->data;

            Link<T> *oldFront = front;
            front = front->next;
            front->prev = nullptr;
            delete oldFront;
            count--;

            return target;
        }
    }

    T removeLast() {
        if (front == nullptr) {
            throw std::logic_error("List is empty");
        } else if (front == back) {
            T target = front->data;

            delete front;
            front = nullptr;
            back = nullptr;
            count--;

            return target;
        } else {
            T target = back->data;

            Link<T> *oldBack = back;
            back = back->prev;
            back->next = nullptr;
            delete oldBack;
            count--;

            return target;
        }
    }

    int size() const { return count; }

    ~LinkedList() {
        while (front != nullptr) {
            removeFirst();
        }
    }

    friend std::ostream &operator<< <>(std::ostream &os,
                                       const LinkedList<T> &list);
    friend struct TestLinkedList;
    friend struct Graph;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const LinkedList<T> &list) {
    Link<T> *temp = list.front;

    while (temp != nullptr) {
        os << temp->data;
        if (temp->next != nullptr) {
            os << " -> ";
        }
        temp = temp->next;
    }

    return os;
}

#endif