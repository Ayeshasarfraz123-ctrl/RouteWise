#ifndef HEAP_H
#define HEAP_H

// Generic heap. The 'better' function decides which item goes on top.
template <typename T>
class Heap {
private:
    T* data;          // array that holds the heap
    int count;        // items stored
    int capacity;     // array size
    bool (*better)(const T&, const T&);   // true if first should be above second

    void swapItems(int i, int j) {
        T temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    // Move an item UP while it is better than its parent
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (better(data[i], data[parent])) {
                swapItems(i, parent);
                i = parent;
            }
            else {
                break;
            }
        }
    }

    // Move an item DOWN while a child is better than it
    void siftDown(int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int best = i;

            if (left < count && better(data[left], data[best]))
                best = left;
            if (right < count && better(data[right], data[best]))
                best = right;

            if (best == i) break;
            swapItems(i, best);
            i = best;
        }
    }

    // Double the array size when it is full
    void grow() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; i++)
            newData[i] = data[i];
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    Heap(bool (*cmp)(const T&, const T&), int startCapacity = 10) {
        better = cmp;
        capacity = startCapacity;
        count = 0;
        data = new T[capacity];
    }

    ~Heap() {
        delete[] data;
    }

    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }

    void insert(const T& value) {
        if (count == capacity) grow();
        data[count] = value;
        siftUp(count);
        count++;
    }

    // Remove the top item. Gives it back in 'out'. False if empty.
    bool extract(T& out) {
        if (count == 0) return false;
        out = data[0];
        count--;
        data[0] = data[count];
        siftDown(0);
        return true;
    }

    // Look at the top item without removing it
    bool peek(T& out) const {
        if (count == 0) return false;
        out = data[0];
        return true;
    }

    // For displaying all items (array order, not sorted)
    const T& getItem(int i) const { return data[i]; }
};

#endif