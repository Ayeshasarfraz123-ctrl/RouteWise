#ifndef DISPATCHOPS_H
#define DISPATCHOPS_H

#include <iostream>
#include <iomanip>
#include <string>
#include "RouteWise.h"
#include "Input.h"
#include "NetworkOps.h"
#include "ParcelOps.h"
#include "Dijkstra.h"
using namespace std;

// =====================================================================
//  Helpers
// =====================================================================

// How many parcels have this status?
inline int countWithStatus(RouteWise& sys, ParcelStatus status) {
    int count = 0;
    for (int i = 0; i < sys.parcels.getCapacity(); i++) {
        Parcel* p = sys.parcels.getValueAt(i);
        if (p != nullptr && p->status == status) count++;
    }
    return count;
}

// Used with the AVL tree to list parcel IDs in sorted order
inline void printIfRegistered(int key, Parcel* p) {
    if (p->status == REGISTERED) cout << key << "  ";
}
inline void printIfProcessing(int key, Parcel* p) {
    if (p->status == PROCESSING) cout << key << "  ";
}

// Take every urgent request out of the heap (in dispatch order).
// 'out' needs room for sys.urgentHeap.size() items. The heap is empty afterwards.
inline int drainUrgent(RouteWise& sys, UrgentRequest* out) {
    int count = 0;
    UrgentRequest r;
    while (sys.urgentHeap.extract(r))
        out[count++] = r;
    return count;
}

// Put them back (skipping one tracking ID, or -1 to keep all)
inline void refillUrgent(RouteWise& sys, const UrgentRequest* items, int count, int skipTrackingId) {
    for (int i = 0; i < count; i++)
        if (items[i].trackingId != skipTrackingId)
            sys.urgentHeap.insert(items[i]);
}

// Remove the newest history entry (used by undo)
inline void popHistory(RouteWise& sys) {
    HistoryEntry h;
    if (sys.history.popFront(h) && h.sequence == sys.historyCounter)
        sys.historyCounter--;
}

// =====================================================================
//  The real work (no typing or printing here)
// =====================================================================

// A parcel asks to be delivered: Registered -> Pending
inline bool requestDelivery(RouteWise& sys, int trackingId, bool urgent, string& error) {
    Parcel* p = sys.parcels.search(trackingId);
    if (p == nullptr) {
        error = "no parcel with that tracking ID";
        return false;
    }
    if (p->status != REGISTERED) {
        error = "parcel is already " + statusToString(p->status) +
            " (only a Registered parcel can be requested)";
        return false;
    }

    sys.arrivalCounter++;
    p->status = PENDING;

    if (urgent) {
        UrgentRequest r;
        r.trackingId = trackingId;
        r.priority = p->priority;
        r.arrivalOrder = sys.arrivalCounter;
        sys.urgentHeap.insert(r);                 // heap: highest priority first
    }
    else {
        sys.normalQueue.enqueue(trackingId);      // queue: first come, first served
    }

    UndoRecord rec = { trackingId, REGISTERED, PENDING, urgent, sys.arrivalCounter };
    sys.undoStack.push(rec);
    return true;
}

// Dispatch the next delivery: urgent first, otherwise the normal queue.
// Pending -> Processing. Returns false if nothing is waiting.
inline bool processNextDelivery(RouteWise& sys, int& trackingId, bool& urgent) {
    int arrival = 0;
    UrgentRequest top;

    if (sys.urgentHeap.extract(top)) {
        trackingId = top.trackingId;
        urgent = true;
        arrival = top.arrivalOrder;
    }
    else if (sys.normalQueue.dequeue(trackingId)) {
        urgent = false;
    }
    else {
        return false;
    }

    Parcel* p = sys.parcels.search(trackingId);
    p->status = PROCESSING;

    HistoryEntry h;
    h.sequence = ++sys.historyCounter;
    h.trackingId = trackingId;
    h.event = PROCESSING;
    sys.history.pushFront(h);                     // newest first

    UndoRecord rec = { trackingId, PENDING, PROCESSING, urgent, arrival };
    sys.undoStack.push(rec);
    return true;
}

// Processing -> Delivered
inline bool markDelivered(RouteWise& sys, int trackingId, string& error) {
    Parcel* p = sys.parcels.search(trackingId);
    if (p == nullptr) {
        error = "no parcel with that tracking ID";
        return false;
    }
    if (p->status != PROCESSING) {
        error = "parcel is " + statusToString(p->status) +
            " (only a Processing parcel can be delivered)";
        return false;
    }

    p->status = DELIVERED;

    HistoryEntry h;
    h.sequence = ++sys.historyCounter;
    h.trackingId = trackingId;
    h.event = DELIVERED;
    sys.history.pushFront(h);

    UndoRecord rec = { trackingId, PROCESSING, DELIVERED, false, 0 };
    sys.undoStack.push(rec);
    return true;
}

// Reverse the most recent status change. Returns false if there is nothing to undo.
inline bool undoLastOperation(RouteWise& sys, UndoRecord& rec) {
    if (!sys.undoStack.pop(rec)) return false;

    Parcel* p = sys.parcels.search(rec.trackingId);

    if (rec.newStatus == PENDING) {
        // The delivery request is cancelled: take it out of the queue or heap
        if (rec.wasUrgent) {
            int n = sys.urgentHeap.size();
            UrgentRequest* items = new UrgentRequest[n + 1];
            int count = drainUrgent(sys, items);
            refillUrgent(sys, items, count, rec.trackingId);
            delete[] items;
        }
        else {
            sys.normalQueue.remove(rec.trackingId);
        }
        if (rec.arrivalOrder == sys.arrivalCounter) sys.arrivalCounter--;
    }
    else if (rec.newStatus == PROCESSING) {
        // The dispatch is reversed: put the parcel back where it was
        if (rec.wasUrgent) {
            UrgentRequest r;
            r.trackingId = rec.trackingId;
            r.priority = p->priority;
            r.arrivalOrder = rec.arrivalOrder;    // same arrival number as before
            sys.urgentHeap.insert(r);
        }
        else {
            sys.normalQueue.enqueueFront(rec.trackingId);   // back to the front of the line
        }
        popHistory(sys);
    }
    else {
        // DELIVERED is reversed
        popHistory(sys);
    }

    p->status = rec.previousStatus;
    return true;
}

// =====================================================================
//  Menu options (typing and printing)
// =====================================================================

// Options 4 and 5
inline void requestOption(RouteWise& sys, bool urgent) {
    if (countWithStatus(sys, REGISTERED) == 0) {
        cout << "No registered parcels are waiting for a delivery request." << endl;
        cout << "Register a parcel first (option 2)." << endl;
        return;
    }
    cout << "--- Add " << (urgent ? "Urgent" : "Normal") << " Delivery Request ---" << endl;
    cout << "Parcels waiting for a request: ";
    sys.avl.inorder(printIfRegistered);
    cout << endl;

    int id = readInt("Tracking ID (0 to cancel): ", 0, 999999999);
    if (inputEnded() || id == 0) return;

    string error;
    if (!requestDelivery(sys, id, urgent, error)) {
        cout << "Could not add the request: " << error << "." << endl;
        return;
    }
    Parcel* p = sys.parcels.search(id);
    if (urgent)
        cout << "Parcel " << id << " added to the URGENT heap (priority " << p->priority
        << "). Urgent requests waiting: " << sys.urgentHeap.size() << endl;
    else
        cout << "Parcel " << id << " added to the NORMAL queue (position "
        << sys.normalQueue.size() << ")." << endl;
}

// Option 6
inline void processNextOption(RouteWise& sys) {
    int id = 0;
    bool urgent = false;
    if (!processNextDelivery(sys, id, urgent)) {
        cout << "No pending deliveries." << endl;
        return;
    }
    Parcel* p = sys.parcels.search(id);
    cout << "Dispatching " << (urgent ? "URGENT" : "NORMAL") << " parcel " << id
        << " (priority " << p->priority << ")." << endl;
    cout << "  " << p->source << " -> " << p->destination << endl << endl;

    // Show the best road route for this delivery
    Graph& g = sys.city;
    int src = g.findLocation(p->source);
    int dst = g.findLocation(p->destination);
    int* path = new int[g.getVertexCount()];
    int length = 0;
    int total = shortestPath(g, src, dst, path, length);
    if (total == -1)
        cout << "Warning: no road connects " << p->source << " and " << p->destination << "." << endl;
    else
        printRoute(g, path, length, total);
    delete[] path;
}

// Option 7
inline void markDeliveredOption(RouteWise& sys) {
    if (countWithStatus(sys, PROCESSING) == 0) {
        cout << "No parcel is being processed right now. Use option 6 first." << endl;
        return;
    }
    cout << "--- Mark Parcel as Delivered ---" << endl;
    cout << "Parcels being processed: ";
    sys.avl.inorder(printIfProcessing);
    cout << endl;

    int id = readInt("Tracking ID (0 to cancel): ", 0, 999999999);
    if (inputEnded() || id == 0) return;

    string error;
    if (!markDelivered(sys, id, error))
        cout << "Could not mark as delivered: " << error << "." << endl;
    else
        cout << "Parcel " << id << " marked as DELIVERED." << endl;
}

// Option 8
inline void undoOption(RouteWise& sys) {
    UndoRecord rec;
    if (!undoLastOperation(sys, rec)) {
        cout << "Nothing to undo." << endl;
        return;
    }
    cout << "Undone: ";
    if (rec.newStatus == PENDING)
        cout << "the delivery request for parcel " << rec.trackingId << " was cancelled." << endl;
    else if (rec.newStatus == PROCESSING)
        cout << "parcel " << rec.trackingId << " was put back in the "
        << (rec.wasUrgent ? "urgent heap." : "normal queue (at the front).") << endl;
    else
        cout << "parcel " << rec.trackingId << " is no longer marked as delivered." << endl;
    cout << "Status of parcel " << rec.trackingId << " is back to "
        << statusToString(rec.previousStatus) << "." << endl;
    cout << "Operations that can still be undone: " << sys.undoStack.size() << endl;
}

// Option 9
inline void displayPendingOption(RouteWise& sys) {
    // ----- urgent: take them out of the heap in order, print, put them back -----
    int n = sys.urgentHeap.size();
    UrgentRequest* items = new UrgentRequest[n + 1];
    int count = drainUrgent(sys, items);
    int nextUrgentId = (count > 0) ? items[0].trackingId : -1;

    cout << "--- URGENT requests (highest priority first, then earliest arrival) ---" << endl;
    if (count == 0) {
        cout << "  (none)" << endl;
    }
    else {
        cout << left << setw(5) << "No." << setw(10) << "Tracking"
            << setw(10) << "Priority" << setw(9) << "Arrival" << "Destination" << endl;
        for (int i = 0; i < count; i++) {
            Parcel* p = sys.parcels.search(items[i].trackingId);
            cout << left << setw(5) << (i + 1) << setw(10) << items[i].trackingId
                << setw(10) << items[i].priority << setw(9) << items[i].arrivalOrder
                << p->destination << endl;
        }
    }
    refillUrgent(sys, items, count, -1);
    delete[] items;

    // ----- normal queue -----
    cout << endl << "--- NORMAL requests (first come, first served) ---" << endl;
    int position = 0;
    int nextNormalId = -1;
    if (sys.normalQueue.isEmpty()) {
        cout << "  (none)" << endl;
    }
    else {
        cout << left << setw(5) << "No." << setw(10) << "Tracking"
            << setw(10) << "Priority" << "Destination" << endl;
        for (Node<int>* cur = sys.normalQueue.getFront(); cur != nullptr; cur = cur->next) {
            position++;
            if (position == 1) nextNormalId = cur->data;
            Parcel* p = sys.parcels.search(cur->data);
            cout << left << setw(5) << position << setw(10) << cur->data
                << setw(10) << p->priority << p->destination << endl;
        }
    }

    cout << endl;
    if (nextUrgentId != -1)
        cout << "Next to be dispatched: parcel " << nextUrgentId << " (urgent)" << endl;
    else if (nextNormalId != -1)
        cout << "Next to be dispatched: parcel " << nextNormalId << " (normal)" << endl;
    else
        cout << "Nothing is waiting to be dispatched." << endl;

    // ----- history (newest first) -----
    cout << endl << "--- Delivery history (newest first, last 10 events) ---" << endl;
    if (sys.history.isEmpty()) {
        cout << "  (no events yet)" << endl;
    }
    else {
        int shown = 0;
        for (Node<HistoryEntry>* cur = sys.history.getHead(); cur != nullptr && shown < 10; cur = cur->next) {
            cout << "  #" << left << setw(4) << cur->data.sequence
                << "parcel " << setw(6) << cur->data.trackingId
                << (cur->data.event == DELIVERED ? "delivered" : "dispatched") << endl;
            shown++;
        }
        if (sys.history.size() > shown)
            cout << "  ... and " << (sys.history.size() - shown) << " older event(s)." << endl;
    }
}

#endif