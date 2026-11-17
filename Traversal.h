#ifndef TRAVERSAL_H
#define TRAVERSAL_H

#include <iostream>
#include "Graph.h"
#include "Queue.h"
#include "Stack.h"
using namespace std;

// BFS from 'start'. Visits nearest locations first (fewest roads).
// 'order' must have room for g.getVertexCount() numbers.
// Returns how many locations were visited.
inline int bfs(const Graph& g, int start, int* order) {
    int n = g.getVertexCount();
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) visited[i] = false;

    Queue<int> q;
    int visitedCount = 0;

    visited[start] = true;      // mark when we put it in the queue
    q.enqueue(start);

    int current;
    while (q.dequeue(current)) {
        order[visitedCount++] = current;

        for (Node<Edge>* e = g.getAdjacency(current).getHead(); e != nullptr; e = e->next) {
            int next = e->data.to;
            if (!visited[next]) {
                visited[next] = true;
                q.enqueue(next);
            }
        }
    }

    delete[] visited;
    return visitedCount;
}

// DFS from 'start'. Goes as deep as possible before coming back.
// Uses our own Stack instead of recursion.
inline int dfs(const Graph& g, int start, int* order) {
    int n = g.getVertexCount();
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) visited[i] = false;

    Stack<int> st;
    int visitedCount = 0;

    st.push(start);

    int current;
    while (st.pop(current)) {
        if (visited[current]) continue;   // may have been reached another way
        visited[current] = true;
        order[visitedCount++] = current;

        // Push neighbours in reverse, so the FIRST neighbour is popped first
        int degree = g.getAdjacency(current).size();
        int* neighbours = new int[degree];
        int k = 0;
        for (Node<Edge>* e = g.getAdjacency(current).getHead(); e != nullptr; e = e->next)
            neighbours[k++] = e->data.to;

        for (int i = degree - 1; i >= 0; i--)
            if (!visited[neighbours[i]]) st.push(neighbours[i]);

        delete[] neighbours;
    }

    delete[] visited;
    return visitedCount;
}

// Can we reach 'target' from 'start' by following roads?
inline bool isReachable(const Graph& g, int start, int target) {
    int n = g.getVertexCount();
    int* order = new int[n];
    int count = bfs(g, start, order);

    bool found = false;
    for (int i = 0; i < count; i++)
        if (order[i] == target) found = true;

    delete[] order;
    return found;
}

// Print: A -> B -> C
inline void printOrder(const Graph& g, const int* order, int count) {
    for (int i = 0; i < count; i++) {
        cout << g.getName(order[i]);
        if (i < count - 1) cout << " -> ";
    }
    cout << endl;
}

#endif