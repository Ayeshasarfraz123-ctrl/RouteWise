#ifndef HASHTABLE_H
#define HASHTABLE_H

// Hash table with linear probing. Key = int (tracking ID), value = pointer.
// NOTE: the table does NOT delete the records it points to.
template <typename V>
class HashTable {
private:
    enum { EMPTY = 0, OCCUPIED = 1, DELETED = 2 };

    struct Slot {
        int key;
        V* value;
        int state;
        Slot() {
            key = 0;
            value = nullptr;
            state = EMPTY;
        }
    };

    Slot* slots;
    int capacity;     // number of drawers
    int count;        // live items
    int used;         // live items + deleted markers
    int lastProbes;   // how many drawers the last search looked at

    int hashFunction(int key) const {
        int h = key % capacity;
        if (h < 0) h += capacity;
        return h;
    }

    // Find the drawer holding this key. Returns -1 if not found.
    int findIndex(int key) {
        int start = hashFunction(key);
        lastProbes = 0;
        for (int i = 0; i < capacity; i++) {
            int idx = (start + i) % capacity;
            lastProbes++;
            if (slots[idx].state == EMPTY) return -1;
            if (slots[idx].state == OCCUPIED && slots[idx].key == key) return idx;
            // DELETED or another key: keep probing
        }
        return -1;
    }

    // Place an item in the first free or deleted drawer (no checks)
    void insertInto(int key, V* value) {
        int start = hashFunction(key);
        for (int i = 0; i < capacity; i++) {
            int idx = (start + i) % capacity;
            if (slots[idx].state != OCCUPIED) {
                if (slots[idx].state == EMPTY) used++;
                slots[idx].key = key;
                slots[idx].value = value;
                slots[idx].state = OCCUPIED;
                count++;
                return;
            }
        }
    }

    // Make the table bigger and place every item again
    void rehash(int newCapacity) {
        Slot* old = slots;
        int oldCapacity = capacity;

        slots = new Slot[newCapacity];
        capacity = newCapacity;
        count = 0;
        used = 0;

        for (int i = 0; i < oldCapacity; i++) {
            if (old[i].state == OCCUPIED)
                insertInto(old[i].key, old[i].value);
        }
        delete[] old;
    }

public:
    HashTable(int startCapacity = 11) {
        capacity = startCapacity;
        slots = new Slot[capacity];
        count = 0;
        used = 0;
        lastProbes = 0;
    }

    ~HashTable() {
        delete[] slots;
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    int size() const { return count; }
    int getCapacity() const { return capacity; }
    int getLastProbes() const { return lastProbes; }

    // Add a record. Returns false if this key already exists.
    bool insert(int key, V* value) {
        if (findIndex(key) != -1) return false;
        if ((used + 1) * 2 > capacity)
            rehash(capacity * 2 + 1);
        insertInto(key, value);
        return true;
    }

    // Find a record. Returns nullptr if not found.
    V* search(int key) {
        int idx = findIndex(key);
        if (idx == -1) return nullptr;
        return slots[idx].value;
    }

    // Remove a record from the table (does not delete the record itself)
    bool remove(int key) {
        int idx = findIndex(key);
        if (idx == -1) return false;
        slots[idx].state = DELETED;
        slots[idx].value = nullptr;
        count--;
        return true;
    }

    // For going through every record (display, cleanup).
    // Returns nullptr if drawer i is empty or deleted.
    V* getValueAt(int i) const {
        if (slots[i].state == OCCUPIED) return slots[i].value;
        return nullptr;
    }
};

#endif