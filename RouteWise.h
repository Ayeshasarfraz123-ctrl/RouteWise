#ifndef ROUTEWISE_H
#define ROUTEWISE_H

#include "LinkedList.h"
#include "Stack.h"
#include "Queue.h"
#include "Heap.h"
#include "HashTable.h"
#include "BST.h"
#include "AVL.h"
#include "Graph.h"
#include "Customer.h"
#include "Parcel.h"
#include "UrgentRequest.h"
#include "UndoRecord.h"
#include "HistoryEntry.h"

// The whole RouteWise system: every data structure lives here, in one place.
struct RouteWise {
    // Records (created with new). The two hash tables OWN them.
    HashTable<Customer> customers;      // lookup by customer ID
    HashTable<Parcel> parcels;          // lookup by tracking ID

    // Ordered parcel indexes (only point to the parcels)
    BST<Parcel> bst;
    AVL<Parcel> avl;

    // Dispatch
    Queue<int> normalQueue;             // normal requests (tracking IDs), first come first served
    Heap<UrgentRequest> urgentHeap;     // urgent requests, highest priority first

    // History and undo
    LinkedList<HistoryEntry> history;   // newest event at the front
    Stack<UndoRecord> undoStack;

    // City map
    Graph city;

    // Counters for automatic numbering
    int nextCustomerId;
    int nextTrackingId;
    int arrivalCounter;
    int historyCounter;

    RouteWise() : urgentHeap(moreUrgent) {
        nextCustomerId = 1;
        nextTrackingId = 1001;
        arrivalCounter = 0;
        historyCounter = 0;
    }

    // Free every customer and parcel record
    ~RouteWise() {
        for (int i = 0; i < parcels.getCapacity(); i++)
            delete parcels.getValueAt(i);
        for (int i = 0; i < customers.getCapacity(); i++)
            delete customers.getValueAt(i);
    }

    RouteWise(const RouteWise&) = delete;
    RouteWise& operator=(const RouteWise&) = delete;
};

#endif