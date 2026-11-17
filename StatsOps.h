#ifndef STATSOPS_H
#define STATSOPS_H

#include <iostream>
#include <iomanip>
#include <string>
#include "RouteWise.h"
#include "Input.h"
#include "DispatchOps.h"
#include "Traversal.h"
using namespace std;

// Smallest height a tree with n records could possibly have
inline int bestPossibleHeight(int n) {
    int height = 0;
    long capacity = 0;           // records that fit in a perfect tree of this height
    while (capacity < n) {
        height++;
        capacity = capacity * 2 + 1;
    }
    return height;
}

// Option 16
inline void compareTreesOption(RouteWise& sys) {
    int n = sys.parcels.size();

    cout << "=== BST vs AVL on your current parcel records ===" << endl;
    if (n == 0) {
        cout << "No parcels registered yet, so there is nothing to compare." << endl;
    }
    else {
        // Collect every tracking ID, then search for each one in both trees
        int* ids = new int[n];
        int count = 0;
        for (int i = 0; i < sys.parcels.getCapacity(); i++) {
            Parcel* p = sys.parcels.getValueAt(i);
            if (p != nullptr) ids[count++] = p->trackingId;
        }

        long bstTotal = 0, avlTotal = 0;
        int bstWorst = 0, avlWorst = 0;
        for (int i = 0; i < count; i++) {
            sys.bst.search(ids[i]);
            int b = sys.bst.getLastSteps();
            sys.avl.search(ids[i]);
            int a = sys.avl.getLastSteps();
            bstTotal += b;
            avlTotal += a;
            if (b > bstWorst) bstWorst = b;
            if (a > avlWorst) avlWorst = a;
        }
        delete[] ids;

        cout << "Parcels indexed: " << n << endl << endl;
        cout << left << setw(26) << "" << setw(10) << "BST" << setw(10) << "AVL" << endl;
        cout << setw(26) << "Tree height" << setw(10) << sys.bst.getHeight()
            << setw(10) << sys.avl.getHeight() << endl;
        cout << setw(26) << "Best possible height" << setw(10) << bestPossibleHeight(n)
            << setw(10) << bestPossibleHeight(n) << endl;
        cout << fixed << setprecision(2);
        cout << setw(26) << "Average search steps" << setw(10) << (double)bstTotal / count
            << setw(10) << (double)avlTotal / count << endl;
        cout.unsetf(ios::fixed);
        cout << setprecision(6);
        cout << setw(26) << "Worst search steps" << setw(10) << bstWorst
            << setw(10) << avlWorst << endl;
        cout << setw(26) << "Rotations done" << setw(10) << "-"
            << setw(10) << sys.avl.getRotations() << endl << endl;

        if (sys.bst.getHeight() > sys.avl.getHeight())
            cout << "The BST grew into a long chain because new tracking IDs keep getting bigger.\n"
            << "The AVL tree rotated to stay balanced, so its searches are shorter." << endl;
        else
            cout << "For this data both trees have the same height." << endl;
    }

    // Demonstration with more records
    cout << endl;
    int demo = readInt("Run a demo with this many sorted records (0 to skip, max 2000): ", 0, 2000);
    if (inputEnded() || demo == 0) return;

    BST<Parcel> demoBst;
    AVL<Parcel> demoAvl;
    for (int key = 1; key <= demo; key++) {
        demoBst.insert(key, nullptr);     // only the keys matter for this demo
        demoAvl.insert(key, nullptr);
    }
    demoBst.search(demo);                 // the last record is the hardest to reach in the BST
    int bstSteps = demoBst.getLastSteps();
    demoAvl.search(demo);
    int avlSteps = demoAvl.getLastSteps();

    cout << endl << "Demo: " << demo << " records inserted in sorted order (1, 2, 3, ...)" << endl;
    cout << left << setw(26) << "" << setw(10) << "BST" << setw(10) << "AVL" << endl;
    cout << setw(26) << "Tree height" << setw(10) << demoBst.getHeight()
        << setw(10) << demoAvl.getHeight() << endl;
    cout << setw(26) << "Steps to find last one" << setw(10) << bstSteps
        << setw(10) << avlSteps << endl;
    cout << setw(26) << "Rotations done" << setw(10) << "-"
        << setw(10) << demoAvl.getRotations() << endl;
}

// Option 17
inline void statisticsOption(RouteWise& sys) {
    Graph& g = sys.city;

    cout << "=========== ROUTEWISE SYSTEM STATISTICS ===========" << endl;
    cout << left;
    cout << setw(34) << "Customers registered" << ": " << sys.customers.size() << endl;
    cout << setw(34) << "Parcels registered" << ": " << sys.parcels.size() << endl;
    cout << setw(34) << "  Registered" << ": " << countWithStatus(sys, REGISTERED) << endl;
    cout << setw(34) << "  Pending" << ": " << countWithStatus(sys, PENDING) << endl;
    cout << setw(34) << "  Processing" << ": " << countWithStatus(sys, PROCESSING) << endl;
    cout << setw(34) << "  Delivered" << ": " << countWithStatus(sys, DELIVERED) << endl;
    cout << endl;
    cout << setw(34) << "Urgent requests waiting (heap)" << ": " << sys.urgentHeap.size() << endl;
    cout << setw(34) << "Normal requests waiting (queue)" << ": " << sys.normalQueue.size() << endl;
    cout << setw(34) << "History events (linked list)" << ": " << sys.history.size() << endl;
    cout << setw(34) << "Operations that can be undone" << ": " << sys.undoStack.size() << endl;
    cout << endl;

    cout << "City network" << endl;
    cout << setw(34) << "  Locations" << ": " << g.getVertexCount() << endl;
    cout << setw(34) << "  Roads" << ": " << g.getEdgeCount() << endl;
    cout << setw(34) << "  Fully connected?" << ": ";
    if (g.getVertexCount() == 0) {
        cout << "no locations yet" << endl;
    }
    else {
        int* order = new int[g.getVertexCount()];
        int reached = bfs(g, 0, order);
        delete[] order;
        if (reached == g.getVertexCount())
            cout << "Yes (every location can be reached from " << g.getName(0) << ")" << endl;
        else
            cout << "No (" << reached << " of " << g.getVertexCount()
            << " locations can be reached from " << g.getName(0) << ")" << endl;
    }
    cout << endl;

    cout << "Data structures" << endl;
    double load = (sys.parcels.getCapacity() > 0)
        ? 100.0 * sys.parcels.size() / sys.parcels.getCapacity() : 0.0;
    cout << fixed << setprecision(1);
    cout << setw(34) << "  Hash table (parcels)" << ": " << sys.parcels.size() << " records in "
        << sys.parcels.getCapacity() << " slots (" << load << "% full)" << endl;
    cout.unsetf(ios::fixed);
    cout << setprecision(6);
    cout << setw(34) << "  BST height" << ": " << sys.bst.getHeight() << endl;
    cout << setw(34) << "  AVL height" << ": " << sys.avl.getHeight() << endl;
    cout << "====================================================" << endl;
}

#endif