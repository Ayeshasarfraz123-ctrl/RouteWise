#ifndef MST_H
#define MST_H

#include <iostream>
#include "Graph.h"
#include "Heap.h"
using namespace std;

// One road chosen for the minimum network
struct MSTEdge {
    int from;
    int to;
    int weight;
};

// Cheaper road = higher priority in the heap
inline bool cheaper(const MSTEdge& a, const MSTEdge& b) {
    return a.weight < b.weight;
}

// Prim's algorithm: grow the network one cheapest road at a time.
// 'edges' must have room for g.getVertexCount() roads.
// Fills edgeCount (roads chosen) and parts (how many separate groups of
// locations exist; 1 means the whole city is connected).
// Returns the total cost of the chosen roads.
inline int minimumSpanningTree(const Graph& g, MSTEdge* edges, int& edgeCount, int& parts) {
    int n = g.getVertexCount();
    bool* inTree = new bool[n];
    for (int i = 0; i < n; i++) inTree[i] = false;

    edgeCount = 0;
    parts = 0;
    int total = 0;

    Heap<MSTEdge> heap(cheaper);

    // Start from every location not yet connected (handles cut-off areas)
    for (int s = 0; s < n; s++) {
        if (inTree[s]) continue;
        parts++;
        inTree[s] = true;

        // Put every road leaving the start into the heap
        for (Node<Edge>* e = g.getAdjacency(s).getHead(); e != nullptr; e = e->next) {
            MSTEdge m;
            m.from = s;
            m.to = e->data.to;
            m.weight = e->data.weight;
            heap.insert(m);
        }

        MSTEdge best;
        while (heap.extract(best)) {
            if (inTree[best.to]) continue;   // would make a loop, skip

            // Take this cheapest road and add the new location
            inTree[best.to] = true;
            edges[edgeCount++] = best;
            total += best.weight;

            for (Node<Edge>* e = g.getAdjacency(best.to).getHead(); e != nullptr; e = e->next) {
                if (!inTree[e->data.to]) {
                    MSTEdge m;
                    m.from = best.to;
                    m.to = e->data.to;
                    m.weight = e->data.weight;
                    heap.insert(m);
                }
            }
        }
    }

    delete[] inTree;
    return total;
}

// Show the chosen roads and the total cost
inline void printMST(const Graph& g, const MSTEdge* edges, int edgeCount, int parts, int total) {
    cout << "Selected connections:" << endl;
    for (int i = 0; i < edgeCount; i++) {
        cout << "  " << g.getName(edges[i].from) << " -- "
            << g.getName(edges[i].to) << " : "
            << edges[i].weight << " km" << endl;
    }
    cout << "Total Network Cost: " << total << " km" << endl;
    if (parts > 1)
        cout << "Note: the city has " << parts
        << " separate parts that no road connects, so one network per part is shown." << endl;
}

#endif