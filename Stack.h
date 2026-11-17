#ifndef STACK_H
#define STACK_H

#include "LinkedList.h"

// Stack built on our own linked list (LIFO)
template <typename T>
class Stack {
private:
    LinkedList<T> list;

public:
    // Put an item on top
    void push(const T& value) {
        list.pushFront(value);
    }

    // Remove the top item. Gives it back in 'out'. False if empty.
    bool pop(T& out) {
        return list.popFront(out);
    }

    // Look at the top item without removing it. False if empty.
    bool peek(T& out) const {
        if (list.isEmpty()) return false;
        out = list.getHead()->data;
        return true;
    }

    bool isEmpty() const { return list.isEmpty(); }
    int size() const { return list.size(); }
    void clear() { list.clear(); }
};

#endif