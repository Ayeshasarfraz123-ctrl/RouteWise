#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <iostream>
#include "Graph.h"
#include "Heap.h"
using namespace std;

const int INF = 1000000000;   // "infinitely far" = not reached yet

// One entry in the heap: a location and the distance we found to it
struct HeapEntry {
    int vertex;
    int distance;
};

// Smaller distance = higher priority (goes to the top of the heap)
inline bool closer(const HeapEntry& a, const HeapEntry& b) {
    return a.distance < b.distance;
}

// Dijkstra's shortest path from 'src' to 'dst'.
// 'path' must have room for g.getVertexCount() numbers.
// Returns the total distance, or -1 if there is no route.
// pathLength tells how many locations are in 'path' (source first).
inline int shortestPath(const Graph& g, int src, int dst, int* path, int& pathLength) {
    int n = g.getVertexCount();
    int* dist = new int[n];       // best distance found so far
    int* parent = new int[n];     // where we came from (to rebuild the route)
    bool* done = new bool[n];     // true once the distance is final

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
        done[i] = false;
    }

    Heap<HeapEntry> heap(closer);
    dist[src] = 0;
    HeapEntry start;
    start.vertex = src;
    start.distance = 0;
    heap.insert(start);

    HeapEntry top;
    while (heap.extract(top)) {
        int v = top.vertex;
        if (done[v]) continue;     // old, outdated entry
        done[v] = true;
        if (v == dst) break;       // shortest distance to destination is final

        // Try to improve the distance to each neighbour
        for (Node<Edge>* e = g.getAdjacency(v).getHead(); e != nullptr; e = e->next) {
            int next = e->data.to;
            int newDist = dist[v] + e->data.weight;
            if (newDist < dist[next]) {
                dist[next] = newDist;
                parent[next] = v;
                HeapEntry entry;
                entry.vertex = next;
                entry.distance = newDist;
                heap.insert(entry);
            }
        }
    }

    int result = -1;
    pathLength = 0;

    if (dist[dst] != INF) {
        result = dist[dst];

        // Walk back from destination to source, then reverse
        int count = 0;
        for (int v = dst; v != -1; v = parent[v])
            path[count++] = v;
        for (int i = 0; i < count / 2; i++) {
            int temp = path[i];
            path[i] = path[count - 1 - i];
            path[count - 1 - i] = temp;
        }
        pathLength = count;
    }

    delete[] dist;
    delete[] parent;
    delete[] done;
    return result;
}

// Show the route like your project document
inline void printRoute(const Graph& g, const int* path, int pathLength, int total) {
    cout << "Recommended Route:" << endl;
    for (int i = 0; i < pathLength; i++) {
        cout << g.getName(path[i]);
        if (i < pathLength - 1) cout << " -> ";
    }
    cout << endl;
    cout << "Total Distance: " << total << " km" << endl;
}

#endif