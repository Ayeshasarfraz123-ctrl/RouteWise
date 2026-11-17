#ifndef QUEUE_H
#define QUEUE_H

#include "LinkedList.h"

// Queue built on our own linked list (FIFO)
template <typename T>
class Queue {
private:
    LinkedList<T> list;

public:
    // Join the back of the line
    void enqueue(const T& value) {
        list.pushBack(value);
    }
    // Go back to the FRONT of the line (used when an operation is undone)
    void enqueueFront(const T& value) {
        list.pushFront(value);
    }
    // Leave from the front of the line. Gives it back in 'out'. False if empty.
    bool dequeue(T& out) {
        return list.popFront(out);
    }
    // Take one specific value out of the line (used when a request is cancelled)
    bool remove(const T& value) {
        return list.removeValue(value);
    }
    // Look at the front item without removing it. False if empty.
    bool peek(T& out) const {
        if (list.isEmpty()) return false;
        out = list.getHead()->data;
        return true;
    }

    bool isEmpty() const { return list.isEmpty(); }
    int size() const { return list.size(); }
    void clear() { list.clear(); }

    // Needed later to display pending deliveries without removing them
    Node<T>* getFront() const { return list.getHead(); }
};

#endif