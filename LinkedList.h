#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// One box in the chain
template <typename T>
struct Node {
    T data;          // the value stored
    Node* next;      // pointer to the next box

    Node(const T& value) {
        data = value;
        next = nullptr;
    }
};

// Singly linked list (we wrote it ourselves, no STL)
template <typename T>
class LinkedList {
private:
    Node<T>* head;   // first box
    Node<T>* tail;   // last box
    int count;       // how many boxes

public:
    LinkedList() {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    // Destructor: frees every box when the list is destroyed
    ~LinkedList() {
        clear();
    }

    // Stop accidental copying (it would cause double-delete bugs)
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
    Node<T>* getHead() const { return head; }

    // Add at the front
    void pushFront(const T& value) {
        Node<T>* n = new Node<T>(value);
        n->next = head;
        head = n;
        if (tail == nullptr) tail = n;   // list was empty
        count++;
    }

    // Add at the end
    void pushBack(const T& value) {
        Node<T>* n = new Node<T>(value);
        if (tail == nullptr) {           // list was empty
            head = n;
            tail = n;
        }
        else {
            tail->next = n;
            tail = n;
        }
        count++;
    }

    // Remove the first item. Gives it back in 'out'. Returns false if empty.
    bool popFront(T& out) {
        if (head == nullptr) return false;
        Node<T>* temp = head;
        out = temp->data;
        head = head->next;
        if (head == nullptr) tail = nullptr;   // list became empty
        delete temp;
        count--;
        return true;
    }
    // Remove the first box that holds this value. Returns false if not found.
    bool removeValue(const T& value) {
        Node<T>* previous = nullptr;
        Node<T>* cur = head;
        while (cur != nullptr) {
            if (cur->data == value) {
                if (previous == nullptr) head = cur->next;   // it was the first box
                else previous->next = cur->next;
                if (cur == tail) tail = previous;            // it was the last box
                delete cur;
                count--;
                return true;
            }
            previous = cur;
            cur = cur->next;
        }
        return false;
    }
    // Is this value in the list?
    bool contains(const T& value) const {
        for (Node<T>* cur = head; cur != nullptr; cur = cur->next) {
            if (cur->data == value) return true;
        }
        return false;
    }

    // Delete everything
    void clear() {
        Node<T>* cur = head;
        while (cur != nullptr) {
            Node<T>* next = cur->next;
            delete cur;
            cur = next;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }
};

#endif